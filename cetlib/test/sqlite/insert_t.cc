#include "cetlib/sqlite/insert.h"

#include <cassert>
#include <string>

int
main()
{
  auto const test = cet::sqlite::detail::values_str(1, "hello", 10);
  assert(test == "1,\"hello\",10");
}
