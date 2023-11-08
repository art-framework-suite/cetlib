// vim: set sw=2 expandtab :

#include "cetlib/sqlite/ConnectionFactory.h"
#include "cetlib/sqlite/Ntuple.h"
#include "cetlib/sqlite/create_table.h"
#include "hep_concurrency/simultaneous_function_spawner.h"

#include <cassert>
#include <filesystem>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace std;
using namespace cet::sqlite;
using namespace hep::concurrency;
namespace fs = std::filesystem;

int
main()
{
  try {
    ConnectionFactory cf;
    // Simple connection to "a.db"
    {
      string const f{"a.db"};
      unique_ptr<Connection> c{cf.make_connection(f)};
      create_table(*c, "onlyOne", column<int>{"numbers"});
      fs::path const p{f};
      assert(fs::exists(p));
    }
    // Check safe operation of an ephemeral connection
    {
      string const f{"ephemeral.db"};
      create_table(*cf.make_connection(f), "onlyOne", column<int>{"numbers"});
      fs::path const p{f};
      assert(fs::exists(p));
    }
    // Simple connection to in-memory database
    {
      unique_ptr<Connection> c{cf.make_connection(":memory:")};
      create_table(*c, "onlyOne", column<int>{"numbers"});
      query_result<int> r;
      r << select("*").from(*c, "onlyOne");
      assert(r.empty());
    }
    // Separate connections to "b.db" and "c.db"
    {
      string const f1{"b.db"};
      string const f2{"c.db"};
      unique_ptr<Connection> c1{cf.make_connection(f1)};
      unique_ptr<Connection> c2{cf.make_connection(f2)};
      create_table(*c1, "separate", column<int>{"numbers"});
      create_table(*c2, "separate", column<int>{"numbers"});
      fs::path const p1{f1};
      fs::path const p2{f2};
      assert(fs::exists(p1));
      assert(fs::exists(p2));
    }
    auto test_colliding_tables = [&cf](string const& dbname) {
      unique_ptr<Connection> c1{cf.make_connection(dbname)};
      unique_ptr<Connection> c2{cf.make_connection(dbname)};
      create_table(*c1, "colliding", column<int>{"numbers"});
      try {
        create_table(*c2, "colliding", column<int>{"numbers"});
      }
      catch (Exception const& e) {
        assert(e.categoryCode() == errors::SQLExecutionError);
        assert(string{e.what()}.find("table colliding already exists") !=
               string::npos);
      }
    };
    // Separate connections to same database "d.db"
    {
      string const f{"d.db"};
      test_colliding_tables(f);
      fs::path const p{f};
      assert(fs::exists(p));
    }
    // Separate connections to same database in-memory database
    test_colliding_tables(":memory:");
    // Test concurrent creation of connections to the same database.
    {
      ConnectionFactory sharedFactory;
      string const f{"e.db"};
      auto makeConnection = [&sharedFactory, &f] {
        sharedFactory.make_connection(f);
      };
      simultaneous_function_spawner launch{repeated_task(10u, makeConnection)};
      fs::path const p{f};
      assert(fs::exists(p));
    }
    // Test concurrent insertion into different tables, using
    // different connections that refer to the same database.
    // - Force flush after each insertion to test thread-safety of
    //   concurrent writes to the same database.
    {
      std::string const f{"f.db"};
      auto c1 = cf.make_connection(f);
      auto c2 = cf.make_connection(f);
      Ntuple<int> n1{*c1, "Odds", {{"Number"}}};
      Ntuple<int> n2{*c2, "Evens", {{"Number"}}};
      std::vector<std::function<void()>> tasks;
      for (unsigned i{1}; i < 6u; ++i) {
        tasks.push_back([&n1, i] {
          n1.insert(2 * i - 1);
          n1.flush();
        });
        tasks.push_back([&n2, i] {
          n2.insert(2 * i);
          n2.flush();
        });
      }
      simultaneous_function_spawner launch{tasks};
      query_result<int> sum_odds;
      sum_odds << select("sum(Number)").from(*c1, "Odds");
      query_result<int> sum_evens;
      sum_evens << select("sum(Number)").from(*c2, "Evens");
      assert(unique_value(sum_odds) == 25);
      assert(unique_value(sum_evens) == 30);
    }
  }
  catch (exception const& e) {
    cerr << e.what() << '\n';
    return 1;
  }
  catch (...) {
    cerr << "Caught unknown exception.\n";
    return 2;
  }
}
