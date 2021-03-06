// vim: set sw=2 expandtab :
#include "cetlib/container_algorithms.h"
#include "cetlib/sqlite/Connection.h"
#include "cetlib/sqlite/ConnectionFactory.h"
#include "cetlib/sqlite/Transaction.h"
#include "cetlib/sqlite/column.h"
#include "cetlib/sqlite/create_table.h"
#include "cetlib/sqlite/insert.h"
#include "cetlib/sqlite/query_result.h"
#include "cetlib/sqlite/select.h"
#include "cetlib_except/exception.h"

#include <array>
#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace cet::sqlite;
using namespace std;

namespace {

  vector<array<string, 2u>> const names{{{"Cindy", "Miller"}},
                                        {{"Billy", "Baker"}}};

  void
  check_table_contents(Connection& c, string const& table_name)
  {
    auto test_results = [](size_t const i, auto const& worker) {
      auto const& [first_name, last_name] = worker;
      assert(first_name == names[i][0]);
      assert(last_name == names[i][1]);
    };
    query_result<string, string> workers;
    workers << select("*").from(c, table_name);
    cet::for_all_with_index(workers, test_results);
  }

} // unnamed namespace

int
main()
{
  try {
    ConnectionFactory factory;
    unique_ptr<Connection> c{factory.make_connection(":memory:")};
    // Test normal transaction
    string const table_name{"Workers"};
    create_table(
      *c, table_name, column<string>{"FirstName"}, column<string>{"LastName"});
    {
      Transaction txn{*c};
      for (auto const& worker : names) {
        insert_into(*c, table_name).values(worker[0], worker[1]);
      }
      txn.commit();
      check_table_contents(*c, table_name);
    }
    string const table_name2{"Workers2"};
    create_table(
      *c, table_name2, column<string>{"FirstName"}, column<string>{"LastName"});
    // Test select before commit is called
    {
      Transaction txn{*c};
      for (auto const& worker : names) {
        insert_into(*c, table_name2).values(worker[0], worker[1]);
      }
      // Calling a SELECT before the transaction is committed implicitly ends
      // the transaciton.
      check_table_contents(*c, table_name2);
      txn.commit(); // Call commit to avoid the error emitted from the
      // ~Transaction().
    }
    // Fail to commit before transaction destroyed (due to exception
    // throw)
    {
      try {
        Transaction txn{*c};
        insert_into(*c, table_name2).values("Johnny", "Davis");
        throw 1.0;
      }
      catch (double) {
      }
      check_table_contents(*c, table_name2);
    }
  }
  catch (cet::exception const& e) {
    cerr << e.what() << '\n';
    return 1;
  }
  catch (...) {
    cerr << "Unknown exception caught.\n";
    return 2;
  }
}
