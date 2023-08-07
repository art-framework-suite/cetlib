cet_test(connection_t SCOPED LIBRARIES PRIVATE
  cetlib::sqlite
  hep_concurrency::simultaneous_function_spawner
  Boost::filesystem
  Threads::Threads)
cet_test(create_table_ddl_t SCOPED LIBRARIES PRIVATE cetlib::sqlite)
cet_test(insert_t SCOPED LIBRARIES PRIVATE cetlib::sqlite)
cet_test(normalize_statement_t SCOPED LIBRARIES PRIVATE Threads::Threads cetlib::sqlite)
cet_test(ntuple_t SCOPED LIBRARIES PRIVATE
  cetlib::sqlite
  hep_concurrency::simultaneous_function_spawner
  Threads::Threads)
cet_test(query_result_t SCOPED LIBRARIES PRIVATE
  cetlib::sqlite
  cetlib::container_algorithms)
cet_test(statistics_t SCOPED LIBRARIES PRIVATE cetlib::sqlite)
cet_test(transaction_t SCOPED
  LIBRARIES PRIVATE cetlib::sqlite cetlib::cetlib
  TEST_PROPERTIES
  PASS_REGULAR_EXPRESSION "Transaction d'tor called before commit was called.")
cet_test(bad_col_t COMPILE_ONLY TEST_PROPERTIES
  PASS_REGULAR_EXPRESSION "template constraint failure.*known_sqlite_type<T>")
cet_test(bad_bind_parameter_t COMPILE_ONLY TEST_PROPERTIES
  PASS_REGULAR_EXPRESSION " note: the required expression ‘get<\\(N - 1\\)>\\(t\\)’ is invalid")
cet_test(bad_select_t COMPILE_ONLY TEST_PROPERTIES
  PASS_REGULAR_EXPRESSION " note: the expression ‘\\(\\.\\.\\. && stringish<T>\\) \\[with T = \\{not_stringish\\}\\]’ evaluated to ‘false’")
