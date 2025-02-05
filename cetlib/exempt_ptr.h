#ifndef cetlib_exempt_ptr_h
#define cetlib_exempt_ptr_h

// ======================================================================
//
// exempt_ptr: A pointer taking no notice of its pointee's lifetime
//             (thus exempt from any role in managing same, and thus
//             copyable with no impact on its pointee)
//
// ----------------------------------------------------------------------
//
// This (not very) smart pointer template is intended as a replacement
// for bare/native/raw/built-in C++ pointers, especially when used to
// communicate with (say) legacy code that traffics in such pointers.
//
// Having such a template provides a standard vocabulary to denote
// non-owning pointers, with no need for further comment or other
// documentation to describe the semantics involved.
//
// As a small bonus, this template's c'tors ensure that all instance
// variables are initialized.
//
// ----------------------------------------------------------------------
//
// Questions:
//   - We have operators ==, !=, < ; should <=  >  >=  be supported?
//   - With which, if any, other smart pointers should this template
//     innately interoperate, and to what degree?
//
// ======================================================================

#include <cstddef>
#include <functional>
#include <type_traits>

namespace cet {

  template <class Element>
  class exempt_ptr;

  namespace detail {
    template <typename T, typename U>
    constexpr bool same_underlying_type{
      std::is_same_v<std::remove_cv_t<T>, std::remove_cv_t<U>>};

    template <typename T, typename U>
    using bool_if_same_underlying_type =
      std::enable_if_t<same_underlying_type<T, U>, bool>;
  }

  template <class E>
  void swap(exempt_ptr<E>&, exempt_ptr<E>&) noexcept;

  template <class E>
  constexpr exempt_ptr<E> make_exempt_ptr(E*) noexcept;

  template <class E>
  constexpr bool operator==(std::nullptr_t, exempt_ptr<E>) noexcept;
  template <class E>
  constexpr bool operator!=(std::nullptr_t, exempt_ptr<E>) noexcept;

  // Deduction guide
  template <class E>
  exempt_ptr(E*) -> exempt_ptr<E>;
}

// ======================================================================

template <class Element>
class cet::exempt_ptr {
public:
  // --- publish our template parameter and variations thereof:
  using element_type = Element;
  using pointer = std::add_pointer_t<Element>;
  using reference = std::add_lvalue_reference_t<Element>;

private:
  template <class P>
  struct is_compatible
    : public std::is_convertible<std::add_pointer_t<P>, pointer> {};

  template <class P>
  static constexpr bool is_compatible_v = is_compatible<P>::value;

public:
  // --- default c'tor:
  constexpr exempt_ptr() noexcept : p{nullptr} {}

  // pointer-accepting c'tors:
  constexpr exempt_ptr(std::nullptr_t) noexcept : p{nullptr} {}
  constexpr explicit exempt_ptr(pointer other) noexcept : p{other} {}

  template <class E2>
  constexpr exempt_ptr(
    E2* other,
    std::enable_if_t<is_compatible_v<E2>>* = nullptr) noexcept
    : p{other}
  {}

  // copying c'tors:
  // use compiler-generated copy c'tor
  template <class E2>
  constexpr exempt_ptr(
    exempt_ptr<E2> const other,
    std::enable_if_t<is_compatible_v<E2>>* = nullptr) noexcept
    : p{other.get()}
  {}

  // pointer-accepting assignments:
  constexpr exempt_ptr&
  operator=(std::nullptr_t) noexcept
  {
    reset(nullptr);
    return *this;
  }

  template <class E2>
  constexpr std::enable_if_t<is_compatible_v<E2>, exempt_ptr&>
  operator=(E2* other) noexcept
  {
    reset(other);
    return *this;
  }

  // copying assignments:
  // use compiler-generated copy assignment
  template <class E2>
  constexpr std::enable_if_t<is_compatible_v<E2>, exempt_ptr&>
  operator=(exempt_ptr<E2> const other) noexcept
  {
    reset(other.get());
    return *this;
  }

  // observers:
  constexpr reference
  operator*() const noexcept
  {
    return *get();
  }
  constexpr pointer
  operator->() const noexcept
  {
    return get();
  }
  constexpr pointer
  get() const noexcept
  {
    return p;
  }
  constexpr bool
  empty() const noexcept
  {
    return get() == nullptr;
  }
  constexpr explicit
  operator bool() const noexcept
  {
    return !empty();
  }

  // modifiers:
  constexpr pointer
  release() noexcept
  {
    pointer old = get();
    reset();
    return old;
  }
  constexpr void
  reset(pointer t = nullptr) noexcept
  {
    p = t;
  }
  void
  swap(exempt_ptr& other) noexcept
  {
    std::swap(p, other.p);
  }

  // comparisons:
  template <typename OtherElement>
  constexpr detail::bool_if_same_underlying_type<Element, OtherElement>
  operator==(exempt_ptr<OtherElement> const y) const noexcept
  {
    return p == y.get();
  }
  template <typename OtherElement>
  constexpr detail::bool_if_same_underlying_type<Element, OtherElement>
  operator!=(exempt_ptr<Element> const y) const noexcept
  {
    return !operator==(y);
  }
  constexpr bool
  operator==(std::nullptr_t) const noexcept
  {
    return p == nullptr;
  }
  constexpr bool
  operator!=(std::nullptr_t) const noexcept
  {
    return !operator==(nullptr);
  }
  template <typename OtherElement>
  constexpr detail::bool_if_same_underlying_type<Element, OtherElement>
  operator<(exempt_ptr<OtherElement> const y) const
  {
    return p < y.get();
  }
  template <typename OtherElement>
  constexpr detail::bool_if_same_underlying_type<Element, OtherElement>
  operator>(exempt_ptr<OtherElement> const y) const
  {
    return y < *this;
  }
  template <typename OtherElement>
  constexpr detail::bool_if_same_underlying_type<Element, OtherElement>
  operator<=(exempt_ptr<OtherElement> const y) const
  {
    return !(y < *this);
  }
  template <typename OtherElement>
  constexpr detail::bool_if_same_underlying_type<Element, OtherElement>
  operator>=(exempt_ptr<OtherElement> const y) const
  {
    return !(*this < y);
  }

private:
  pointer p;

}; // exempt_ptr<>

// ======================================================================
// non-member functions:

// ----------------------------------------------------------------------
// non-member swap:

template <class E>
inline void
cet::swap(exempt_ptr<E>& x, exempt_ptr<E>& y) noexcept
{
  x.swap(y);
}

// ----------------------------------------------------------------------
// non-member make_exempt_ptr:

template <class E>
constexpr cet::exempt_ptr<E>
cet::make_exempt_ptr(E* p) noexcept
{
  return exempt_ptr<E>{p};
}

// ----------------------------------------------------------------------
// non-member (in)equality comparison:

template <class E>
constexpr bool
cet::operator==(std::nullptr_t, exempt_ptr<E> const y) noexcept
{
  return y == nullptr;
}

template <class E>
constexpr bool
cet::operator!=(std::nullptr_t, exempt_ptr<E> const y) noexcept
{
  return y != nullptr;
}

// ----------------------------------------------------------------------
// non-member ordering:

// ======================================================================

#endif /* cetlib_exempt_ptr_h */

// Local Variables:
// mode: c++
// End:
