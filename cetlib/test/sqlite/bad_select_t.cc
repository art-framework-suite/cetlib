#include "cetlib/sqlite/select.h"

#include <utility>

struct not_stringish {};

int
main()
{
  std::ignore = cet::sqlite::select(not_stringish{});
}
