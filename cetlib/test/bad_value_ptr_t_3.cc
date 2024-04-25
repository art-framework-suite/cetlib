#include "cetlib/value_ptr.h"

#include <cstdint>

struct simple_base {
  std::int32_t n;
  std::uint16_t s;
  std::uint8_t c;
  virtual ~simple_base() noexcept = default;
};

struct simple_sub : simple_base {
  std::int32_t m;
  simple_sub*
  clone()
  {
    return new simple_sub(*this);
  }
};

int
main()
{
  cet::value_ptr<simple_base, cet::default_copy<simple_base>> p(
    new simple_sub{});
}
