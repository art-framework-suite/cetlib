#include "cetlib/pow.h"
#include <string>

struct bad_data {
  std::string identifier;
  constexpr banana() : name("something"){};
};

int
main()
{
  bad_data* something = new bad_data{};
  cet::pow<13>(something);
  delete something;
}
