#ifndef cetlib_sqlite_ConnectionFactory_h
#define cetlib_sqlite_ConnectionFactory_h
// vim: set sw=2 expandtab :

// ====================================================================
// ConnectionFactory
//
// This factory is the only way in which a cet::sqlite::Connection
// object can be created.
//
// For each database filename, there is an associated mutex that is
// shared across Connections referring to the same filename.  Whenever
// an insertion is being performed across multiple Connections, the
// shared mutex guarantees that the database is updated in a
// thread-safe manner.
//
// This facility is necessary because the locking mechanisms that
// SQLite relies on are deficient for NFS systems.
//
// The typical use case is:
//
//   ConnectionFactory factory;
//   auto c1 = factory.make(":memory:");
//   auto c2 = factory.make<MyDBOpenPolicy>("a.db", openPolicyArgs...);
//   auto c3 = factory.make(":memory:");
//
// In the above, c1 and c3 will refer to the same in-memory database.
// To enable thread-safe insertion of data into the DB, consider using
// the Ntuple facility.
// ====================================================================

#include "cetlib/sqlite/Connection.h"
#include "cetlib/sqlite/detail/DefaultDatabaseOpenPolicy.h"

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <utility>

namespace cet::sqlite {

  class ConnectionFactory {
  public:
    template <typename DatabaseOpenPolicy = detail::DefaultDatabaseOpenPolicy,
              typename... PolicyArgs>
    auto make_connection(std::string const& file_name, PolicyArgs&&...)
      -> std::unique_ptr<Connection>;

  private:
    std::unordered_map<std::string, std::weak_ptr<std::recursive_mutex>>
      databaseLocks_;
    std::recursive_mutex mutex_;
  };

  template <typename DatabaseOpenPolicy, typename... PolicyArgs>
  auto
  ConnectionFactory::make_connection(std::string const& filename,
                                     PolicyArgs&&... policyArgs)
    -> std::unique_ptr<Connection>
  {
    // Implementation a la Herb Sutter's favorite 10-liner
    std::lock_guard sentry{mutex_};
    // Note: Convert the weak_ptr to a shared_ptr using the member
    // function lock(), this is not an operation on the mutex.
    auto shared_ptr_to_mutex = databaseLocks_[filename].lock();
    if (!shared_ptr_to_mutex) {
      using namespace std::string_literals;
      databaseLocks_[filename] = shared_ptr_to_mutex =
        std::make_shared<std::recursive_mutex>();
    }

    // We can not just call std::make_unique<Connection>(...) because the
    // constructor for Connection we need to call is private, and make_unique
    //  is not a friend... and the syntax for making the right function template
    //  be a friend is non-obvious.
    Connection* pc = new Connection(
      filename,
      shared_ptr_to_mutex,
      DatabaseOpenPolicy{std::forward<PolicyArgs>(policyArgs)...});
    return std::unique_ptr<Connection>(pc);
  }

} // namespace cet::sqlite

#endif /* cetlib_sqlite_ConnectionFactory_h */

// Local Variables:
// mode: c++
// End:
