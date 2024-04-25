#include "cetlib/bit_manipulation.h"
#include <cstdlib>

using cet::bit_number;
using cet::bit_number_v;
using cet::bit_size;
using cet::bit_size_v;
using cet::circ_lshift;
using cet::left_bits;
using cet::left_bits_v;
using cet::right_bits;
using cet::right_bits_v;

void
ensure(int which, bool claim)
{
  if (not claim)
    std::exit(which);
}

int
main()
{
  using uint = unsigned int;
  using ulong = unsigned long;

  ensure(1, bit_size<uint>::value == sizeof(uint) * 8);
  ensure(2, bit_size_v<uint> == sizeof(uint) * 8);

  ensure(11, bit_size<ulong>::value == sizeof(ulong) * 8);
  ensure(12, bit_size_v<ulong> == sizeof(ulong) * 8);

  ensure(21, bit_number<uint, 0>::value == 1u);
  ensure(22, bit_number<uint, 1>::value == 2u);
  ensure(23, bit_number<uint, 3>::value == 8u);
  ensure(24, bit_number_v<uint, 3> == 8u);

  ensure(31, right_bits<uint, 0>::value == 1u);
  ensure(32, right_bits<uint, 1>::value == 3u);
  ensure(33, right_bits<uint, 3>::value == 15u);

  ensure(41,
         (left_bits<uint, 0>::value ^ uint(-1)) == right_bits<uint, 32>::value);
  ensure(42,
         (left_bits<uint, 1>::value ^ uint(-1)) == right_bits<uint, 31>::value);
  ensure(43,
         (left_bits<uint, 3>::value ^ uint(-1)) == right_bits<uint, 29>::value);
  ensure(44, (left_bits_v<uint, 3> ^ uint(-1)) == right_bits_v<uint, 29>);

  for (uint k = 0, pow2 = 1; k != bit_size<uint>::value; ++k, pow2 *= 2)
    ensure(21 + k, circ_lshift(1u, k) == pow2);

  return 0;

} // main()
