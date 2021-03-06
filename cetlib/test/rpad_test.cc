#define BOOST_TEST_MODULE (rpad test)

#include "boost/test/unit_test.hpp"
#include "cetlib/rpad.h"
#include <string>

BOOST_AUTO_TEST_SUITE(rpad_test)

BOOST_AUTO_TEST_CASE(dont_pad_test)
{
  std::string const s0("abc");

  std::string s1 = cet::rpad(s0, 0, '*');
  BOOST_TEST(s1 == s0);

  std::string s2 = cet::rpad(s0, 1, '*');
  BOOST_TEST(s2 == s0);

  std::string s3 = cet::rpad(s0, 2, '*');
  BOOST_TEST(s3 == s0);

  std::string s4 = cet::rpad(s0, 3, '*');
  BOOST_TEST(s4 == s0);
}

BOOST_AUTO_TEST_CASE(do_pad_test)
{
  std::string const s0("abc");

  std::string s1 = cet::rpad(s0, 4);
  BOOST_TEST(s1 == s0 + ' ');

  std::string s2 = cet::rpad(s0, 5, '*');
  BOOST_TEST(s2 == s0 + std::string(2, '*'));
}

BOOST_AUTO_TEST_SUITE_END()
