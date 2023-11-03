// vim: set sw=2 expandtab :

#include "cetlib/sqlite/ConnectionFactory.h"
#include "cetlib/sqlite/Exception.h"
#include "cetlib/sqlite/Ntuple.h"
#include "cetlib/sqlite/helpers.h"
#include "cetlib/sqlite/select.h"
#include "hep_concurrency/simultaneous_function_spawner.h"

#include "sqlite3.h"

#include <assert.h>
#include <cstring>
#include <functional>
#include <iostream>
#include <vector>

// We do not use Boost Unit Test here because we want the product into
// which this moves to be independent of Boost.

using namespace cet::sqlite;
using namespace std;

void
test_with_new_database(Connection& c)
{
  cout << "start test_with_new_database\n";
  assert(c);
  Ntuple<double, string> xx{c, "xx", {{"x", "txt"}}};
  cout << "end test_with_new_database\n";
}

void
test_with_matching_table(Connection& c)
{
  cout << "start test_with_matching_table\n";
  assert(c);
  Ntuple<double, string> xx{c, "xx", {{"x", "txt"}}};
  cout << "end test_with_matching_table\n";
}

template <class... ARGS>
void
test_with_colliding_table(Connection& c,
                          array<string, sizeof...(ARGS)> const& names)
{
  cout << "start test_with_colliding_table\n";
  assert(c);
  try {
    Ntuple<ARGS...> xx{c, "xx", names};
    assert("Failed throw for mismatched table" == nullptr);
  }
  catch (Exception const& x) {
  }
  catch (...) {
    assert("Threw wrong exception for mismatched table" == nullptr);
  }
  cout << "end test_with_colliding_table\n";
}

void
test_filling_table(Connection& c)
{
  cout << "start test_filling_table\n";
  assert(c);
  constexpr int nrows{903};
  {
    Ntuple<int, double> nt{c, "zz", {{"i", "x"}}, false, 100};
    for (int i = 0; i < nrows; ++i) {
      nt.insert(i, 1.5 * i);
    }
  }
  query_result<int> nmatches;
  nmatches << select("count(*)").from(c, "zz");
  // Check that there are 'nrows' rows in the database.
  assert(unique_value(nmatches) == nrows);
  cout << "end test_filling_table\n";
}

void
test_parallel_filling_table(Connection& c)
{
  cout << "start test_parallel_filling_table\n";
  assert(c);
  constexpr int nrows_per_thread{100};
  constexpr unsigned nthreads{10};
  string const tablename{"zz"};
  {
    Ntuple<int, double> nt{c,
                           tablename,
                           {{"i", "x"}},
                           true,
                           60}; // Force flushing after 60 insertions.
    vector<function<void()>> tasks;
    for (unsigned i{}; i < nthreads; ++i) {
      tasks.emplace_back([i, &nt] {
        for (unsigned j{}; j < nrows_per_thread; ++j) {
          auto const j1 = j + i * 100;
          nt.insert(j1, 1.5 * j1);
        }
      });
    }
    hep::concurrency::simultaneous_function_spawner sfs{tasks};
  }
  query_result<int> nmatches;
  nmatches << select("count(*)").from(c, tablename);
  assert(unique_value(nmatches) == nrows_per_thread * nthreads);
  cout << "end test_parallel_filling_table\n";
}

void
test_column_constraint(Connection& c)
{
  cout << "start test_column_constraint\n";
  assert(c);
  Ntuple<column<int, primary_key>, double> nt{c, "u", {{"i", "x"}}};
  auto const& ref = detail::create_table_ddl(
    "u", column<int, primary_key>{"i"}, column<double>{"x"});
  assert(hasTableWithSchema(c, "u", ref));
  cout << "end test_column_constraint\n";
}

void
test_file_create(ConnectionFactory& cf)
{
  cout << "start test_file_create\n";
  string const filename{"myfile.db"};
  remove(filename.c_str());
  unique_ptr<Connection> c{cf.make_connection(filename)};
  {
    Ntuple<int, double, int> table{*c, "tab1", {{"i", "x", "k"}}, false, 5};
    for (size_t i = 0; i < 103; ++i) {
      table.insert(i, 0.5 * i, i * i);
    }
  }
  query_result<int> cnt;
  cnt << select("count(*)").from(*c, "tab1");
  assert(unique_value(cnt) == 103);
  cout << "end test_file_create\n";
}

void
test_filling_database(ConnectionFactory& cf)
{
  cout << "start test_filling_database\n";
  // Create a connection for a new database with a small maximum page count.
  string filename = "database_for_testing_full_ntuple.db";
  remove(filename.c_str());
  auto c = cf.make_connection(filename);
  char* errmsg = nullptr;
  int rc =
    sqlite3_exec(*c, "PRAGMA main.max_page_count=2", nullptr, nullptr, &errmsg);
  if (rc != SQLITE_OK) {
    sqlite3_free(errmsg);
    cerr << "Failed to reset max_page_count\n";
    abort();
  }

  // Create an Ntuple with forced flushing on every insert.
  Ntuple<int, double> table{*c, "silly", {{"k", "x"}}};
  // max_rows is the maximum number of rows that can be inserted into a
  // database with the max_page_count of 2. This was determined under
  // SQLite version 3.40.1.
  size_t const max_rows = 247;
  for (size_t i = 0; i < max_rows; ++i) {
    table.insert(10, 1.5);
    table.flush();
    assert(not table.full());
  }
  // We should now have max_rows elements in the table.
  query_result<int> nmatches;
  nmatches << select("count(*)").from(*c, "silly");
  assert(unique_value(nmatches) == max_rows);

  // The database should now be full. The next insert should not insert new
  // data. It should also set 'full' to true. And we should still have max_rows
  // elements in the table.
  cout << "Inserting final record\n";
  table.insert(20, 2.5);
  table.flush();
  assert(table.full());
  nmatches << select("count(*)").from(*c, "silly");
  assert(unique_value(nmatches) == max_rows);

  cout << "end test_filling_database\n";
}

int
main()
try {
  const char* fname{"no_such_file.db"};
  // If there is a database in the directory, delete it.
  remove(fname);

  // Now make a database we are sure is empty.
  ConnectionFactory cf;
  auto c = cf.make_connection(fname);

  test_with_new_database(*c);
  test_with_matching_table(*c);
  test_with_colliding_table<int, double>(*c, {{"y", "txt"}});
  test_with_colliding_table<int, double>(*c, {{"x", "text"}});
  test_with_colliding_table<int, int>(*c, {{"x", "txt"}});
  test_with_colliding_table<int, double, int>(*c, {{"x", "txt", "z"}});
  test_with_colliding_table<int>(*c, {{"x"}});
  test_filling_table(*c);
  test_parallel_filling_table(*c);
  test_column_constraint(*c);
  test_file_create(cf);
  test_filling_database(cf);
}
catch (exception const& x) {
  cout << x.what() << endl;
  return 1;
}
catch (...) {
  cout << "Unknown exception caught\n";
  return 2;
}
