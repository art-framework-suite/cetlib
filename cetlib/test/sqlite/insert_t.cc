// vim: set sw=2 expandtab :

#include "cetlib/sqlite/Connection.h"
#include "cetlib/sqlite/ConnectionFactory.h"
#include "cetlib/sqlite/column.h"
#include "cetlib/sqlite/create_table.h"
#include "cetlib/sqlite/insert.h"

#include <cassert>
#include <iostream>
#include <string>

using namespace cet::sqlite;

void
test_value_string_construction()
{
  std::string result =
    cet::sqlite::detail::values_str(1, "hello, world", 10.25);
  assert(result == "1,\"hello, world\",10.25");
}

void
test_insertion()
{
  const char* fname{"created_by_insert_t.db"};
  remove(fname);

  // Create an empty database with a tiny maximum page count.
  ConnectionFactory cf;
  auto pconn = cf.make_connection(fname);
  assert(pconn);        // the returned pointer is non-null
  assert(pconn->get()); // the contained sqlite3 db is non-null
  char* errmsg = nullptr;
  auto rc = sqlite3_exec(
    *pconn, "PRAGMA main.max_page_count=2;", nullptr, nullptr, &errmsg);
  if (rc != SQLITE_OK) {
    std::cout << errmsg;
    sqlite3_free(errmsg);
    abort();
  }
  std::cout << "Connection is created\n";

  // Create an empty table with 1 column of type int.
  std::string tablename("Workers");
  create_table(*pconn, tablename, column<int>{"x"});
  std::cout << "Table created\n";

  // The table should be empty.
  query_result<int> res;
  res << select("x").from(*pconn, tablename);
  assert(res.empty());
  std::cout << "Table is empty\n";

  // Insert a value. The table should then have one value.
  insert_into(*pconn, tablename).values(10);
  res << select("x").from(*pconn, tablename);
  assert(res.size() == 1);
  std::cout << "First value inserted\n";

  // Insert more values. This should cause our tiny database to be full.
  // This value of max_rows was determined under SQLite version 3.40.1.
  std::size_t const max_rows = 526;
  for (std::size_t i = 0; i != max_rows - 1; ++i) {
    insert_into(*pconn, tablename).values(10);
  }
  res << select("x").from(*pconn, tablename);
  assert(res.size() == max_rows);
  std::cout << max_rows << " values (total) inserted.\n";

  // Inserting one more value fails.
  try {
    insert_into(*pconn, tablename).values(20);
    res << select("x").from(*pconn, tablename);
    std::cerr << "Failed to throw required exception\n";
    abort();
  }
  catch (...) {
    std::cout << "Inserting into full DB failed as expected\n";
  }
  assert(res.size() == max_rows);
  for (auto i = res.begin(); i != res.end(); ++i) {
    assert(std::get<0>(*i) == 10);
  }
}

int
main()
try {
  test_value_string_construction();
  test_insertion();
}
catch (std::exception const& x) {
  std::cout << x.what() << '\n';
  return 1;
}
catch (...) {
  std::cout << "Unknown exception caught\n";
  return 2;
}
