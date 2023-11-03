#ifndef cetlib_sqlite_Ntuple_h
#define cetlib_sqlite_Ntuple_h
// vim: set sw=2 expandtab :

// =====================================================================
//
// Ntuple
//
// The Ntuple class template is an interface for inserting into an
// SQLite database in a type-safe and thread-safe manner.  It is not
// intended for this facility to provide facilities for making SQLite
// database queries.  For querying, consider using the
// 'cet::sqlite::select' utilities.
//
//
// Construction
// ------------
//
// The c'tor receives a reference to a cet::sqlite::Connection object,
// that must outlive the life of the Ntuple.  It is the responsibility
// of the user to manage the Connection's lifetime.  A Connection is
// created by a call to ConnectionFactory::make.  Please see the
// documentation in cetlib/sqlite/Connection(Factory).h.
//
// In addition to the Connection object, the Ntuple c'tor receives an
// SQLite table name and a set of column names in the form of an
// std::array object.
//
// See the Ntuple definition below for the full c'tor signatures.
//
// N.B. Constructing an Ntuple in a multi-threaded context is NOT
//      thread-safe if done in parallel with any operations being
//      performed on the same database elsewhere.
//
// Template arguments
// ------------------
//
// The template arguments supplied for the Ntuple type indicate the
// type of the column.  The following Ntuple specifications are
// identical in semantics:
//
//   Ntuple<int, double, string> n1 {...};
//   Ntuple<column<int>, column<double>, column<string>> n2 {...};
//
// both of which indicate three columns with SQLite types of INTEGER,
// NUMERIC, and TEXT, respectively.  Specifying the 'column' template
// is necessary ONLY when a column constraint is desired (e.g.):
//
//   Ntuple<column<int, primary_key>, double, string> n3 {...};
//
// The names of the columns are provided as a c'tor argument (see
// below).
//
// Intended use
// ------------
//
// There are two public modifying functions that can be called:
//
//    Ntuple::insert(...)
//    Ntuple::flush()
//
// Calling 'insert' will add items to the internal buffer until the
// maximum buffer size has been reached, at which point, the contents
// of the buffer will be automatically flushed to the SQLite database.
//
// Calling 'flush' is necessary only when no more items will be
// inserted into the Ntuple AND querying of the Ntuple is required
// immediately thereafter.  The Ntuple d'tor automatically calls flush
// whenever the Ntuple object goes out of scope.
//
// Filling the disk (or database)
// ------------------------------
//
// Each time data are flushed, it is possible that the associated SQLite
// database might get "full". If this happens, the Ntuple stops collecting
// new data; the 'insert' function becomes a no-op, as does 'flush'.
//
// Examples of use
// ---------------
//
//    using namespace cet::sqlite;
//
//    auto c = existing_connection_factory.make("languages.db");
//    Ntuple<string, string> langs {c, "europe", {"Country","Language"}};
//    langs.insert("Germany", "German");
//    langs.insert("Switzerland", "German");
//    langs.insert("Switzerland", "Italian");
//    langs.insert("Switzerland", "French");
//    langs.insert("Switzerland", "Romansh");
//    langs.flush();
//
//    query_result<string> ch;
//    ch << select("Languange").from(c,
//    "europe").where("Country='Switzerland'");
//    // see cet::sqlite::select documentation regarding using query_result.
//
// -----------------------------------------------------------
//
// Technical notes:
//
//   In principle, one could use a concurrent container to prevent
//   having to lock whenever an insert is done.  However, since a
//   flush occurs whenever the buffer max is reached, the buffer must
//   be protected from any modification until the flush is complete.
//   A lock is therefore inevitable.  We could probably optimize by
//   using an atomic variable to protect against modification only
//   when a flush is being done.  This is a potential optimization to
//   keep in mind.
// ===========================================================

#include "cetlib/sqlite/Connection.h"
#include "cetlib/sqlite/Transaction.h"
#include "cetlib/sqlite/column.h"
#include "cetlib/sqlite/detail/bind_parameters.h"
#include "cetlib/sqlite/helpers.h"

#include "sqlite3.h"

#include <iostream>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

namespace cet::sqlite {

  template <typename... Args>
  class Ntuple {
    // Types
  public:
    // Elements of row are unique_ptr's so that it is possible to bind to a
    // null parameter.
    template <typename T>
    using element_t =
      std::unique_ptr<typename sqlite::permissive_column<T>::element_type>;
    using row_t = std::tuple<element_t<Args>...>;
    static constexpr auto nColumns = std::tuple_size_v<row_t>;
    using name_array = sqlite::name_array<nColumns>;
    // Special Member Functions
  public:
    ~Ntuple() noexcept
    {
      if (flush_no_throw() != SQLITE_OK) {
        std::cerr << "SQLite step failure while flushing.\n";
      }
      sqlite3_finalize(insert_statement_);
    }

    Ntuple(Connection& connection,
           std::string const& name,
           name_array const& columns,
           bool overwriteContents = false,
           std::size_t bufsize = 1000ull);
    Ntuple(Ntuple const&) = delete;
    Ntuple& operator=(Ntuple const&) = delete;
    // API
  public:
    std::string const&
    name() const
    {
      return name_;
    }

    // insert a new record into the buffer. If the buffer becomes full,
    // flush() is called internally. If the database becomes full further
    // calls to insert() become a no-op.
    void insert(Args const...);

    // flush() inserts the buffered records into the controlled database.
    // If the database becomes full further calls to flush() become a no-op.
    void flush();

    bool full() const;

    // Implementation details
  private:
    static constexpr auto iSequence = std::make_index_sequence<nColumns>();
    // This is the ctor that does all of the work.  It exists so that
    // the Args... and column-names array can be expanded in parallel.
    template <std::size_t... I>
    Ntuple(Connection& db,
           std::string const& name,
           name_array const& columns,
           bool overwriteContents,
           std::size_t bufsize,
           std::index_sequence<I...>);
    int flush_no_throw();
    // Member data
  private:
    // Protects all of the data members.
    std::recursive_mutex mutex_{};
    Connection& connection_;
    std::string const name_;
    std::size_t const max_;
    std::vector<row_t> buffer_{};
    sqlite3_stmt* insert_statement_{nullptr};
    bool full_{false};
  };

} // cet::sqlite

template <typename... Args>
template <std::size_t... I>
cet::sqlite::Ntuple<Args...>::Ntuple(Connection& connection,
                                     std::string const& name,
                                     name_array const& cnames,
                                     bool const overwriteContents,
                                     std::size_t const bufsize,
                                     std::index_sequence<I...>)
  : connection_{connection}, name_{name}, max_{bufsize}
{
  std::lock_guard sentry{mutex_};
  assert(connection);
  sqlite::createTableIfNeeded(connection,
                              overwriteContents,
                              name,
                              sqlite::permissive_column<Args>{cnames[I]}...);
  std::string sql{"INSERT INTO "};
  sql += name;
  sql += " VALUES (?";
  for (std::size_t i = 1; i < nColumns; ++i) {
    sql += ",?";
  }
  sql += ")";
  int const rc{sqlite3_prepare_v2(
    connection_.get(), sql.c_str(), sql.size(), &insert_statement_, nullptr)};
  if (rc != SQLITE_OK) {
    auto const ec = sqlite3_step(insert_statement_);
    throw sqlite::Exception{sqlite::errors::SQLExecutionError}
      << "Failed to prepare insertion statement.\n"
      << "Return code: " << ec << '\n';
  }
  buffer_.reserve(bufsize);
}

template <typename... Args>
cet::sqlite::Ntuple<Args...>::Ntuple(Connection& connection,
                                     std::string const& name,
                                     name_array const& cnames,
                                     bool const overwriteContents,
                                     std::size_t const bufsize)
  : Ntuple{connection, name, cnames, overwriteContents, bufsize, iSequence}
{}

template <typename... Args>
void
cet::sqlite::Ntuple<Args...>::insert(Args const... args)
{
  std::lock_guard sentry{mutex_};
  if (full_)
    return;

  if (buffer_.size() == max_) {
    flush();
  }
  buffer_.emplace_back(std::make_unique<Args>(args)...);
}

template <typename... Args>
int
cet::sqlite::Ntuple<Args...>::flush_no_throw()
{
  // Guard against any modifications to the buffer, which is about to
  // be flushed to the database.
  std::lock_guard sentry{mutex_};

  // If the database is full, do nothing and pretend all is well.
  if (full_)
    return SQLITE_OK;

  int rc = -1;
  try {
    // Despite the name, Connection::flush_no_throw() can throw. This can
    // happen, for example, if the binding of a parameter to the SQLite
    // statement for the insertion fails.
    rc = connection_.flush_no_throw<nColumns>(buffer_, insert_statement_);
  }
  catch (Exception const& exc) {
    std::string msg = exc.what();
    // Detect the magic number 13, which is the value of
    // SQLITE_FULL.
    // TODO: when we get to rely on C++20, this can be done
    // with
    //     msg.ends_with("13")
    // rather than this mess.
    if ((msg.size() > 3) && (msg.substr(msg.size() - 2) == "13")) {
      rc = SQLITE_FULL;
    } else {
      rc = SQLITE_ERROR;
    }
  }
  catch (...) {
    rc = SQLITE_ERROR;
  }

  switch (rc) {
  case SQLITE_FULL:
    buffer_.clear();
    full_ = true;
    // We'll pretend all is well...
    return SQLITE_OK;
  case SQLITE_DONE:
    buffer_.clear();
    return SQLITE_OK;
  default:
    // We have encountered some sort of error,
    // other than SQLITE_FULL.
    break;
  }
  // We will only get here if we encountered an error other than
  // SQLITE_FULL.
  return rc;
}

template <typename... Args>
void
cet::sqlite::Ntuple<Args...>::flush()
{
  // No lock here -- lock held by flush_no_throw;
  if (flush_no_throw() != SQLITE_OK) {
    throw sqlite::Exception{sqlite::errors::SQLExecutionError}
      << "SQLite step failure while flushing.";
  }
}

template <typename... Args>
bool
cet::sqlite::Ntuple<Args...>::full() const
{
  return full_;
}

#endif /* cetlib_sqlite_Ntuple_h */

// Local Variables:
// mode: c++
// End:
