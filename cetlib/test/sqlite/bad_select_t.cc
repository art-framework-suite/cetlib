#include "cetlib/sqlite/select.h"

struct not_stringish {};

int
main()
{
  (void)cet::sqlite::select(not_stringish{});
}
