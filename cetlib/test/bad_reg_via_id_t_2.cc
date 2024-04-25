#include "cetlib/registry_via_id.h"

using cet::registry_via_id;

/*
 * This test is meant to fail due to the registry being used and having
 * an id() function for its value type, but the id() function returns the
 * wrong type.
 */

struct thing {
  int i;
  thing(int i = 0) : i(i) {}
  float
  id() const
  {
    return i;
  }
  bool
  operator==(int other) const
  {
    return i == other;
  }
};

int
main()
{
  registry_via_id<char, thing>::put(thing{});
}
