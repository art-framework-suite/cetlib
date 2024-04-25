#include "cetlib/LibraryManager.h"
#include "cetlib/test/LibraryManagerTestFunc.h"

#include <vector>

using namespace cet

  int
  main()
{
  LibraryManager lm;
  std::vector<int> nums;
  lm.getLoadableLibraries(nums.begin());
}
