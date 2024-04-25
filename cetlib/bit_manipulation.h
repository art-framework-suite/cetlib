#ifndef cetlib_bit_manipulation_h
#define cetlib_bit_manipulation_h

#include <concepts>
#include <cstddef>
#include <limits>

namespace cet {

  /// struct bit_size<U>.
  template <std::unsigned_integral U>
  struct bit_size {
    static constexpr std::size_t value = std::numeric_limits<U>::digits;
  };

  template <std::unsigned_integral U>
  constexpr std::size_t bit_size_v = bit_size<U>::value;

  /// struct bit_number<U, n>.
  template <std::unsigned_integral U, std::size_t n>
  struct bit_number {
    static constexpr std::size_t value =
      (n < bit_size_v<U>) ? U(1u) << n : U(0u);
  };

  template <std::unsigned_integral U, std::size_t n>
  constexpr std::size_t bit_number_v = bit_number<U, n>::value;

  /// struct right_bits<U, n>.
  template <std::unsigned_integral U, std::size_t n>
  struct right_bits {
    static constexpr U value = ((n + 1) < bit_size_v<U>) ?
                                 bit_number_v<U, n + 1> - static_cast<U>(1u) :
                                 ~0u;
  };

  template <std::unsigned_integral U, std::size_t n>
  constexpr U right_bits_v = right_bits<U, n>::value;

  // struct left_bits<U, n>.
  template <std::unsigned_integral U, std::size_t n>
  struct left_bits {
    static constexpr U value =
      (n <= bit_size_v<U>) ? ~right_bits_v<U, bit_size_v<U> - n> : U(-1);
  };

  template <std::unsigned_integral U, std::size_t n>
  constexpr U left_bits_v = left_bits<U, n>::value;

  // U circ_lshift<U>().
  template <std::unsigned_integral U>
  inline constexpr U
  circ_lshift(U X, U n)
  {
    constexpr std::size_t nbits = bit_size_v<U>;
    constexpr std::size_t mask = nbits - 1ul;
    n %= nbits;
    return (X << n) | (X >> (nbits - n) & mask);
  }
}
#endif /* cetlib_bit_manipulation_h */

// Local Variables:
// mode: c++
// End:
