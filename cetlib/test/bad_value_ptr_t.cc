#include "cetlib/value_ptr.h"

#include <cstdint>

union simple_base {
public:
  std::int32_t n;
  std::uint16_t s;
  std::uint8_t c;
  void
  clone()
  {}
};

int
main()
{
  int my_int = 3;
  cet::value_ptr<simple_base> p(&my_int);
}
