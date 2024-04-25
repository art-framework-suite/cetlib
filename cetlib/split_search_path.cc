#include "cetlib/split_search_path.h"

#include <ranges>

std::vector<std::string>
cet::split_search_path(std::string const& search_path)
{
  std::vector<std::string> results;
  for (auto&& item : search_path | std::views::split(':')) {
    results.emplace_back(item.begin(), item.end());
  }
  return results;
}
