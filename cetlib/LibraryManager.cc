#include "cetlib/LibraryManager.h"

#include "cetlib/container_algorithms.h"
#include "cetlib/detail/plugin_search_path.h"
#include "cetlib/plugin_libpath.h"
#include "cetlib/search_path.h"
#include "cetlib/shlib_utils.h"
#include "cetlib_except/demangle.h"
#include "cetlib_except/exception.h"

extern "C" {
#include <dlfcn.h>
}

#include <algorithm>
#include <filesystem>
#include <iterator>
#include <regex>
#include <sstream>
#include <vector>

namespace {
  inline std::string
  maybe_trim_shlib_prefix(std::string const& spec)
  {
    return spec.find(cet::shlib_prefix()) == 0 ?
             spec.substr(cet::shlib_prefix().size()) :
             spec;
  }
}

cet::LibraryManager::LibraryManager(search_path search_path,
                                    std::string lib_type)
  : LibraryManager{std::move(search_path),
                   std::move(lib_type),
                   defaultPatternStem()}
{}

cet::LibraryManager::LibraryManager(search_path search_path,
                                    std::string lib_type,
                                    std::string pattern)
  : search_path_{detail::plugin_search_path(std::move(search_path))}
  , lib_type_{std::move(lib_type)}
  , pattern_stem_{std::move(pattern)}
{
  std::vector<std::string> matches;
  search_path_.find_files(
    shlib_prefix() + pattern_stem_ + lib_type_ + dllExtPattern(), matches);

  // Note the use of reverse iterators here: files found earlier in
  // the vector will therefore overwrite those found later, which is
  // what we want from "search path"-type behavior.
  std::for_each(matches.rbegin(), matches.rend(), [this](auto const& match) {
    this->lib_loc_map_inserter(match);
  });

  // Build the spec to long library name translation table.
  for (auto const& p : lib_loc_map_) {
    spec_trans_map_inserter(p);
  }

  // Build the fast good-translation table.
  for (auto const& p : spec_trans_map_) {
    good_spec_trans_map_inserter(p);
  }
}

cet::LibraryManager::LibraryManager(std::string lib_type)
  : LibraryManager{std::move(lib_type), defaultPatternStem()}
{}

cet::LibraryManager::LibraryManager(std::string lib_type, std::string pattern)
  : LibraryManager{search_path{plugin_libpath(), std::nothrow},
                   std::move(lib_type),
                   std::move(pattern)}
{}

cet::LibraryManager::~LibraryManager() = default;

size_t
cet::LibraryManager::getLoadableLibraries(std::vector<std::string>& list) const
{
  return getLoadableLibraries(std::back_inserter(list));
}

size_t
cet::LibraryManager::getLoadedLibraries(std::vector<std::string>& list) const
{
  return getLoadedLibraries(std::back_inserter(list));
}

size_t
cet::LibraryManager::getValidLibspecs(std::vector<std::string>& list) const
{
  return getValidLibspecs(std::back_inserter(list));
}

std::pair<std::string, std::string>
cet::LibraryManager::getSpecsByPath(std::string const& lib_loc) const
{
  // pair<short_spec,full_spec>
  std::pair<std::string, std::string> result;
  for (auto const& entry : spec_trans_map_) {
    auto const& spec = maybe_trim_shlib_prefix(entry.first);
    auto const& paths = entry.second;

    auto const path_iter = paths.find(lib_loc);
    if (path_iter != paths.end()) {
      if (spec.find("/") != std::string::npos)
        result.second = spec;
      else
        result.first = spec;
    }
    if (!result.first.empty() && !result.second.empty())
      break;
  }

  return result;
}

void
cet::LibraryManager::loadAllLibraries() const
{
  for (auto const& lib : lib_loc_map_) {
    if (get_lib_ptr(lib.second) == nullptr) {
      throw exception("Configuration")
        << "Unable to load requested library " << lib.second << "\n"
        << demangle_message(dlerror()) << "\n";
    }
  }
}

bool
cet::LibraryManager::libraryIsLoaded(std::string const& path) const
{
  return lib_ptr_map_.find(path) != lib_ptr_map_.end();
}

bool
cet::LibraryManager::libraryIsLoadable(std::string const& path) const
{
  // TODO: If called with any frequency, this should be made more
  // efficient.
  for (auto const& lib : lib_loc_map_) {
    if (path == lib.second) {
      return true;
    }
  }
  return false;
}

void
cet::LibraryManager::lib_loc_map_inserter(std::string const& path)
{
  lib_loc_map_[std::filesystem::path(path).filename().native()] = path;
}

void
cet::LibraryManager::spec_trans_map_inserter(
  lib_loc_map_t::value_type const& entry)
{
  // First obtain short spec.
  std::string const e_str = "([^_]+)_" + lib_type_ + dllExtPattern() + '$';
  std::regex const e{e_str};
  std::match_results<std::string::const_iterator> match_results;
  if (std::regex_search(entry.first, match_results, e)) {
    spec_trans_map_[match_results[1]].insert(entry.second);
  } else {
    throw exception("LogicError")
      << "Internal error in spec_trans_map_inserter for entry " << entry.first
      << " with pattern " << e_str;
  }
  // Next, convert library filename to full libspec.
  std::ostringstream lib_name;
  std::ostream_iterator<char, char> oi{lib_name};
  std::regex_replace(oi,
                     entry.first.begin(),
                     entry.first.end(),
                     std::regex{"_+"},
                     std::string{"/"});
  std::regex const stripper{"^lib(.*)/" + lib_type_ + "\\..*$"};
  std::string const lib_name_str{lib_name.str()};
  if (std::regex_search(lib_name_str, match_results, stripper)) {
    spec_trans_map_[match_results[1]].insert(entry.second);
  } else {
    throw exception("LogicError")
      << "Internal error in spec_trans_map_inserter stripping "
      << lib_name.str();
  }
}

void
cet::LibraryManager::good_spec_trans_map_inserter(
  spec_trans_map_t::value_type const& entry)
{
  if (entry.second.size() == 1) {
    good_spec_trans_map_[entry.first] = *(entry.second.begin());
  }
}

void*
cet::LibraryManager::get_lib_ptr(std::string const& lib_loc) const
{
  lib_ptr_map_t::const_iterator const it{lib_ptr_map_.find(lib_loc)};
  if (it == lib_ptr_map_.cend() || it->second == nullptr) {
    dlerror();
    void* ptr = dlopen(lib_loc.c_str(), RTLD_LAZY | RTLD_GLOBAL);
    lib_ptr_map_[lib_loc] = ptr;
    return ptr;
  }
  return it->second;
}

void*
cet::LibraryManager::getSymbolByLibspec_(std::string const& libspec,
                                         std::string const& sym_name,
                                         bool should_throw_on_dlsym) const
{
  if (libspec.find("_") != std::string::npos) {
    // Plugin names (and hence the class name) cannot contain an underscore.
    throw exception("LogicError", "IllegalUnderscore.")
      << "Library specification \"" << libspec
      << "\" contains an illegal underscore.\n"
      << "The class name and path to it may not contain an underscore. "
      << "If this is a configuration error, plase correct it; "
      << "if the module's class name or its location within its "
      << "enclosing package really do have an underscore this situation "
      << "must be rectified.\n";
  }
  auto const trans = good_spec_trans_map_.find(libspec);
  if (trans == good_spec_trans_map_.cend()) {
    // No good translation => zero or too many
    std::ostringstream error_msg;
    error_msg << "Library specification \"" << libspec << "\"";
    auto const bad_trans = spec_trans_map_.find(libspec);
    if (bad_trans != spec_trans_map_.cend()) {
      error_msg << " corresponds to multiple libraries:\n";
      copy_all(bad_trans->second,
               std::ostream_iterator<std::string>(error_msg, "\n"));
    } else {
      auto const& path_name = search_path_.showenv();
      error_msg << " does not correspond to any library";
      if (!path_name.empty()) {
        error_msg << " in " << path_name;
      }
      error_msg << " of type \"" << lib_type_ << "\"\n";
    }
    throw exception("Configuration") << error_msg.str();
  }
  return getSymbolByPath_(trans->second, sym_name, should_throw_on_dlsym);
}

void*
cet::LibraryManager::getSymbolByPath_(std::string const& lib_loc,
                                      std::string const& sym_name,
                                      bool should_throw_on_dlsym) const
{
  void* result = nullptr;
  void* lib_ptr = get_lib_ptr(lib_loc);
  if (lib_ptr == nullptr) {
    throw exception("Configuration")
      << "Unable to load requested library " << lib_loc << "\n"
      << demangle_message(dlerror()) << "\n";
  } else { // Found library
    dlerror();
    result = dlsym(lib_ptr, sym_name.c_str());
    char const* error = dlerror();
    if (error != nullptr) { // Error message
      result = nullptr;
      if (should_throw_on_dlsym) {
        throw exception("Configuration")
          << "Unable to load requested symbol " << demangle_symbol(sym_name)
          << " from library " << lib_loc << "\n"
          << demangle_message(error) << "\n";
      }
    }
  }
  return result;
}

// Local Variables:
// mode: c++
// End:
