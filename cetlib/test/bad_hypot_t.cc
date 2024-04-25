#include "cetlib/hypot.h"

#include <string>

using namespace std::string_literals;

int
main()
{
  cet::hypot(5, 12);
  cet::hypot("lorem"s, "ipsum"s);
}
