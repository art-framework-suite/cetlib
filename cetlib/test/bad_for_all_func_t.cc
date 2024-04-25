#include "cetlib/container_algorithms.h"
#include <functional>
#include <vector>

int
main()
{
  std::vector<int> x{1, 2, 3, 5, 7, 11, 13, 17};
  cet::for_all(x, []() {});
}
