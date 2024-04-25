#include "cetlib/sqlite/detail/bind_parameters.h"

using namespace cet::sqlite::detail;

struct not_gettable {};

int
main()
{
  bind_parameters<not_gettable, 2>::bind(nullptr, {});
}
