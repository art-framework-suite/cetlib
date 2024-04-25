#include "cetlib/search_path.h"
#include <vector>

int
main()
{
  cet::search_path const sp{"void"};
  std::vector<int> nums;
  sp.find_files("random", std::begin(nums));
}
