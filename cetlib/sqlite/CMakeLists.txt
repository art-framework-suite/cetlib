
find_package(SQLite3 REQUIRED EXPORT)
find_package(hep_concurrency REQUIRED EXPORT)

cet_make_library(
  SOURCE
    Connection.cc
    Exception.cc
    Transaction.cc
    detail/DefaultDatabaseOpenPolicy.cc
    detail/bind_parameters.cc
    detail/normalize_statement.cc
    exec.cc
    helpers.cc
    statistics.cc
  LIBRARIES
    PUBLIC
      cetlib::container_algorithms
      cetlib_except::cetlib_except
      SQLite::SQLite3
)

install_headers(SUBDIRS detail)
install_source(SUBDIRS detail)
