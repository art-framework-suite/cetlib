#ifndef cetlib_registry_via_id_h
#define cetlib_registry_via_id_h

// ======================================================================
//
// registry_via_id<K,V>: A singleton std::map<K,V> requiring that V::id()
//                       exists and yields values of type K
//
// ======================================================================

#include "cetlib_except/exception.h"

#include <concepts>
#include <iterator>
#include <map>
#include <type_traits>

namespace cet {
  namespace detail {
    // concept requiring that V have a method called id() and that it returns
    // the same type as K.
    template <class K, class V>
    concept has_id = requires(V val) {
                       {
                         val.id()
                         } -> std::same_as<K>;
                     };
  }
  template <class K, class V>
  class registry_via_id;
}

// ======================================================================

template <class K, class V>
class cet::registry_via_id {
  // non-instantiable (and non-copyable, just in case):
  registry_via_id() = delete;
  registry_via_id(registry_via_id const&) = delete;
  void operator=(registry_via_id const&) = delete;

public:
  using collection_type = std::map<K const, V>;
  using key_type = typename collection_type::key_type;
  using mapped_type = typename collection_type::mapped_type;
  using value_type = typename collection_type::value_type;
  using size_type = typename collection_type::size_type;
  using const_iterator = typename collection_type::const_iterator;

  // observers:
  static bool
  empty()
  {
    return the_registry_().empty();
  }
  static size_type
  size()
  {
    return the_registry_().size();
  }

  // iterators:
  static const_iterator
  begin()
  {
    return the_registry_().begin();
  }
  static const_iterator
  end()
  {
    return the_registry_().end();
  }
  static const_iterator
  cbegin()
  {
    return the_registry_().cbegin();
  }
  static const_iterator
  cend()
  {
    return the_registry_().cend();
  }

  // mutators:

  //////////////////
  // put()

  // 1. A single value.
  static K put(V const& value)
    requires cet::detail::has_id<K, V>;

  // 2. A range of values.
  template <class FwdIt>
  static std::enable_if_t<
    std::is_same_v<typename std::iterator_traits<FwdIt>::value_type,
                   mapped_type>>
  put(FwdIt begin, FwdIt end);

  // 3. A range of std::pair<K, V>. For each pair:
  //
  //      first == second.id()
  //
  //    is a prerequisite.
  template <class FwdIt>
  static std::enable_if_t<
    std::is_same_v<typename std::iterator_traits<FwdIt>::value_type,
                   value_type>>
  put(FwdIt begin, FwdIt end);

  // 4. A collection of std::pair<K, V>. For each pair:
  //
  //      first == second.id()
  //
  //    is a prerequisite.
  static void put(collection_type const& c);
  //////////////////

  // accessors:
  static collection_type const&
  get() noexcept
  {
    return the_registry_();
  }
  static V const& get(K const& key);
  static bool get(K const& key, V& value) noexcept;

private:
  // encapsulated singleton:
  static collection_type&
  the_registry_()
  {
    static collection_type the_registry;
    return the_registry;
  }

}; // registry_via_id<>

//////////////////
// put()

// 1.
template <class K, class V>
K
cet::registry_via_id<K, V>::put(V const& value)
  requires cet::detail::has_id<K, V>
{
  K id = value.id();
  the_registry_().emplace(id, value);
  return id;
}

// 2.
template <class K, class V>
template <class FwdIt>
inline auto
cet::registry_via_id<K, V>::put(FwdIt b, FwdIt e) -> std::enable_if_t<
  std::is_same_v<typename std::iterator_traits<FwdIt>::value_type, mapped_type>>
{
  for (; b != e; ++b)
    put(*b);
}

// 3.
template <class K, class V>
template <class FwdIt>
inline auto
cet::registry_via_id<K, V>::put(FwdIt b, FwdIt e) -> std::enable_if_t<
  std::is_same_v<typename std::iterator_traits<FwdIt>::value_type, value_type>>
{
  the_registry_().insert(b, e);
}

// 4.
template <class K, class V>
inline void
cet::registry_via_id<K, V>::put(collection_type const& c)
{
  put(c.cbegin(), c.cend());
}

// ----------------------------------------------------------------------
// get() overloads:

template <class K, class V>
V const&
cet::registry_via_id<K, V>::get(K const& key)
{
  const_iterator it = the_registry_().find(key);
  if (it == the_registry_().end())
    throw cet::exception("cet::registry_via_id")
      << "Key \"" << key << "\" not found in registry";
  return it->second;
}

template <class K, class V>
bool
cet::registry_via_id<K, V>::get(K const& key, V& value) noexcept
{
  bool result;
  const_iterator it = the_registry_().find(key);
  if (it == the_registry_().end()) {
    result = false;
  } else {
    value = it->second;
    result = true;
  }
  return result;
}

#endif /* cetlib_registry_via_id_h */

// Local Variables:
// mode: c++
// End:
