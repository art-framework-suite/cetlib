#include "cetlib/container_algorithms.h"

template <typename T>
struct bad_container {
  T t;
};

int
main()
{
  bad_container<int> bc;
  cet::for_all(bc, [](int b) { return b; });
}
