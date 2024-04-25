#ifndef cetlib_split_search_path_h
#define cetlib_split_search_path_h

#include <ranges>
#include <string>
#include <vector>

namespace cet {
  std::vector<std::string> split_search_path(std::string const& search_path);
}

#endif /* cetlib_split_search_path_h */

// Local Variables:
// mode: c++
// End:
