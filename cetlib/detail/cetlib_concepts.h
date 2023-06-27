#ifndef cetlib_concepts_h
#define cetlib_concepts_h

#include <concepts>
#include <iterator>
#include <string>

namespace cet::detail {
  template <typename T>
  concept cet_pointer = std::is_pointer_v<T>;

  template <class OutIter>
  concept valid_iter = std::output_iterator<OutIter, std::string>;

  template <class T>
  concept is_arithmetic = std::integral<T> || std::floating_point<T>;
}

#endif
