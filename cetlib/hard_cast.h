#ifndef cetlib_hard_cast_h
#define cetlib_hard_cast_h

#include "cetlib/detail/cetlib_concepts.h"
#include <concepts>
#include <cstring>

namespace cet {
  // For use when only a C++ {dynamic,static,reinterpret}_cast is not
  // sufficient to the task. The only case of this known currently is
  // when using dlopen, dlsym, etc. and a void * must be cast to a
  // function pointer.
  template <detail::cet_pointer PTR_T>
  PTR_T hard_cast(void* src);

  template <detail::cet_pointer PTR_T>
  void hard_cast(void* src, PTR_T& dest);
}

template <cet::detail::cet_pointer PTR_T>
inline PTR_T
cet::hard_cast(void* src)
{
  PTR_T dest;
  hard_cast(src, dest);
  return dest;
}

template <cet::detail::cet_pointer PTR_T>
inline void
cet::hard_cast(void* src, PTR_T& dest)
{
  memcpy(&dest, &src, sizeof(PTR_T));
}
#endif /* cetlib_hard_cast_h */

// Local Variables:
// mode: c++
// End:
