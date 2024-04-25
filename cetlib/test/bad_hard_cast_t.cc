#include "cetlib/hard_cast.h"

int
main()
{
  [[maybe_unused]] int value = 23;
  int* result = cet::hard_cast<int>(value);
  return *result;
}
