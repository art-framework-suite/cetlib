#ifndef cetlib_hypot_h
#define cetlib_hypot_h

// ======================================================================
//
// hypot: Checked and unchecked Euclidean planar hypotenuse calculations
//
// ======================================================================

#include <cmath>
#include <limits>
#include <type_traits>
#include <utility>

#include "cetlib/detail/cetlib_concepts.h"
#include <concepts>
// ----------------------------------------------------------------------

namespace cet {
  template <class T>
    requires(detail::is_arithmetic<T>)
  T hypot(T x, T y);

  template <class T>
    requires(detail::is_arithmetic<T>)
  T unchecked_hypot(T x, T y);

  template <class T>
    requires(detail::is_arithmetic<T>)
  T checked_hypot(T x, T y);
}

// ----------------------------------------------------------------------
// unchecked_hypot<>:

template <class T>
  requires(cet::detail::is_arithmetic<T>)
inline T cet::unchecked_hypot(T x, T y)
{
  return std::hypot(x, y);
}

// ----------------------------------------------------------------------
// checked_hypot<>:

template <class T>
  requires(cet::detail::is_arithmetic<T>)
T cet::checked_hypot(T x, T y)
{

  if (std::isinf(x) || std::isinf(y))
    return std::numeric_limits<T>::infinity();
  else if (std::isnan(x) || std::isnan(y))
    return std::numeric_limits<T>::quiet_NaN();
  else
    return unchecked_hypot(x, y);

} // checked_hypot<>(,)

// ----------------------------------------------------------------------
// hypot<>:

template <class T>
  requires(cet::detail::is_arithmetic<T>)
inline T cet::hypot(T x, T y)
{
  return checked_hypot(x, y);
}

#endif /* cetlib_hypot_h */

// Local Variables:
// mode: c++
// End:
