#include "cetlib/sqlite/column.h"

using namespace cet::sqlite;

struct no_good_type {};

int
main()
{
  using mycol = column<no_good_type>;
}
