#include "cetlib/registry_via_id.h"

using cet::registry_via_id;

/*
 * This test is meant to fail due to the registry being used without an
 * id() function in the value type.
 */

struct thing {
  int i;
  thing(int i = 0) : i(i) {}
  bool
  operator==(int other) const
  {
    return i == other;
  }
};

int
main()
{
  registry_via_id<float, thing>::put(thing{});
}
