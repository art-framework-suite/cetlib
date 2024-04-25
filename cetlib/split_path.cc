// ======================================================================
//
// split_path: Split the string 'path' into components delimited by a
//             single colon. Adjacent colons result in an empty string.
//
// ======================================================================

#include "cetlib/split_path.h"

#include "cetlib/split_search_path.h"

#include <algorithm>

void
cet::split_path(std::string const& path, std::vector<std::string>& components)
{
  if (path.empty()) {
    components.clear();
  } else {
    auto results = split_search_path(path);
    std::move(results.begin(), results.end(), components.end());
  }
}
