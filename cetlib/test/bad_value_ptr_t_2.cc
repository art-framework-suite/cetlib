#include "cetlib/value_ptr.h"

#include <cstdint>

struct simple_base {
  std::int32_t n;
  std::uint16_t s;
  std::uint8_t c;
  virtual ~simple_base() noexcept = default;
};

struct simple_sub : public simple_base {
  std::int32_t m;
};

int
main()
{
  simple_sub s1;
  cet::value_ptr<simple_base> p(&s1);
}
