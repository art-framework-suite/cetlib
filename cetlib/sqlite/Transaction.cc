#include "cetlib/sqlite/Transaction.h"
#include "cetlib/sqlite/Exception.h"

#include "sqlite3.h"

#include <cassert>
#include <iostream>

namespace {
  class set_to_null_when_done {
  public:
    explicit set_to_null_when_done(sqlite3*& db) noexcept : db_{db} {}
    ~set_to_null_when_done() noexcept { db_ = nullptr; }

  private:
    sqlite3*& db_;
  };
}

cet::sqlite::Transaction::Transaction(sqlite3* db) noexcept(false) : db_{db}
{
  assert(db_);
  int const rc{sqlite3_exec(db_, "BEGIN;", nullptr, nullptr, nullptr)};
  if (rc != SQLITE_OK) {
    throw Exception{errors::SQLExecutionError}
      << "Failed to start SQLite transaction due to status code " << rc << ":\n"
      << sqlite3_errmsg(db_) << '\n';
  }
}

cet::sqlite::Transaction::~Transaction() noexcept
{
  // We can't throw an exception from our destructor, so we just emit
  // an error message.
  if (db_) {
    sqlite3_exec(db_, "ROLLBACK;", nullptr, nullptr, nullptr);
    std::cerr << "Transaction d'tor called before commit was called.\n"
              << "The associated SQLite database may be full.\n";
  }
}

void
cet::sqlite::Transaction::commit() noexcept(false)
{
  assert(db_);
  set_to_null_when_done sentry{db_};
  int const rc{sqlite3_exec(db_, "COMMIT;", nullptr, nullptr, nullptr)};
  if (rc != SQLITE_OK) {
    throw Exception{errors::SQLExecutionError}
      << "Failed to commit SQLite transaction due to status code " << rc
      << ":\n"
      << sqlite3_errmsg(db_) << '\n';
  }
}
