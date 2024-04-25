#include "catch2/catch_test_macros.hpp"

#include "cetlib/split_search_path.h"
#include <ranges>

using namespace cet;
using std::string;
using std::vector;

namespace {
  using std::literals::string_literals::operator""s;
  using results_type = decltype(split_search_path(""));
}

TEST_CASE("test_empty_path")
{
  CHECK(split_search_path(""s).empty());
}

TEST_CASE("test_single_entry_path")
{
  auto const search_path = "ABC_D/EFG/H"s;
  results_type ref = {search_path};
  CHECK(split_search_path(search_path) == ref);
}

TEST_CASE("test_adjacent_colons")
{
  auto const search_path = "A::B"s;
  results_type ref = {"A"s, ""s, "B"s};
  CHECK(split_search_path(search_path) == ref);
}

TEST_CASE("test_leading_colon")
{
  auto const search_path = ":BOO"s;
  results_type ref = {""s, "BOO"s};
  CHECK(split_search_path(search_path) == ref);
}

TEST_CASE("test_trailing_colon")
{
  auto const search_path = "A:"s;
  results_type ref = {"A"s, ""s};
  CHECK(split_search_path(search_path) == ref);
}

TEST_CASE("test_typical_use")
{
  results_type const path_elements = {"/p/gcc/v4_5_1/Linux64/lib64"s,
                                      "/p/gcc/v4_5_1/Linux64/lib"s,
                                      "/p/root/v5_26_00d/lib"s,
                                      "/p/cmake/v2_6_4/."s,
                                      "/p/cppunit/v1_12_1/slf5.x86_64.a1/lib"s};

#if __cpp_lib_ranges_join_with >= 202202L &&                                   \
  __cpp_lib_ranges_to_container >= 202202L
  // * Satisfied by LLVM/Clang >=17.
  // * *NOT* satisfied by GCC <=13.
  auto const path = std::ranges::to<std::string>(
    path_elements | std::ranges::views::join_with(":"s));
#else
  string path = path_elements[0];
  for (auto const& pe : path_elements | std::views::drop(1)) {
    path += ":"s + pe;
  }
#endif

  CHECK(split_search_path(path) == path_elements);
}
