#include "cetlib/detail/provide_file_path.h"
#include <cassert>
#include <filesystem>

std::string
cet::detail::provide_file_path(char const* filename)
{
  assert(filename);
  std::filesystem::path const p{filename};
  return absolute(p).native();
}
