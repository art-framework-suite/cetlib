# Audit of Headers for Concepts/Constraints

## Progress

* [x] `BasicPluginFactory.h`
  * Refer to reasons listed below `PluginFactory.h`
* [x] `HorizontalRule.h`
  * No use of templates.
* [x] `LibraryManager.h`
* [x] `MD5Digest.h`
  * No use of templates.
* [x] `PluginFactory.h`
  * Concepts would provide very little value and decrease the readability of existing code.
  * Equivalent value would be provided by an extra line of comment.
* [x] `PluginTypeDeducer.h`
  * The only use of templates here is unconstrained by design.
* [x] `ProvideFilePathMacro.h`
  * No use of templates.
* [x] `ProvideMakePluginMacros.h`
  * No use of templates.
* [x] `base_converter.h`
  * No use of templates.
* [x] `bit_manipulation.h`
* [x] `bit_manipulation_cxx20.h`
* [x] `bold_fontify.h`
  * No use of templates.
* [x] `canonical_number.h`
  * No use of templates.
* [x] `canonical_string.h`
  * No use of templates.
* [x] `column_width.h`
  * No use of templates.
* [x] `compiler_macros.h`
  * No use of templates.
* [x] `container_algorithms.h`
  * Diagnostics are already of sufficient clarity.
* [x] `cpu_timer.h`
  * No use of templates.
* [x] `crc32.h`
  * No use of templates.
* [x] `detail/ostream_handle_impl.h`
  * Diagnostics are already of sufficient clarity.
* [x] `detail/plugin_search_path.h`
  * No use of templates.
* [x] `detail/provide_file_path.h`
  * No use of templates.
* [x] `detail/wrapLibraryManagerException.h`
  * No use of templates.
* [x] `exempt_ptr.h`
  * Value proposition is unclear.
* [x] `filepath_maker.h`
  * No use of templates.
* [x] `filesystem.h`
  * No use of templates.
* [x] `getenv.h`
  * No use of templates.
* [x] `hard_cast.h`
* [x] `hypot.h`
* [x] `include.h`
  * No use of templates.
* [x] `includer.h`
  * No use of templates.
* [x] `loadable_libraries.h`
  * No use of templates.
* [x] `lpad.h`
  * No use of templates.
* [x] `map_vector.h`
  * Value is not meant to be constrained.
* [x] `maybe_ref.h`
  * Class is ripe for deprecation and replacement by std::optional [c++17]
* [x] `metaprogramming.h`
  * Candidate for replacement when supporting standards below C++20 is no longer necessary.
* [x] `name_of.h`
  * Concepts not applicable
* [x] `no_delete.h`
  * No use of templates.
* [x] `nybbler.h`
  * No use of templates.
* [x] `os_libpath.h`
  * No use of templates.
* [x] `ostream_handle.h`
  * Diagnostics are already of sufficient clarity.
* [x] `parsed_program_options.h`
  * No use of templates.
* [x] `plugin_libpath.h`
  * No use of templates.
* [x] `pow.h`
  * Diagnostics are already of sufficient clarity.
  * Encapsulating all acceptable use cases would appear to be overly
    complex.
* [x] `propagate_const.h`
  * Candidate for replacement using std::propagate_const if it gets integrated into a standard.
* [x] `registry.h`
  * Concepts not necessary.
* [x] `registry_via_id.h`
* [x] `replace_all.h`
  * No use of templates.
* [x] `rpad.h`
  * No use of templates.
* [x] `search_path.h`
* [x] `sha1.h`
  * No use of templates.
* [x] `shlib_utils.h`
  * No use of templates.
* [x] `simple_stats.h`
  * Only use of templates is for a streaming operator. Use of templates is unnecessary.
* [x] `split.h`
  * Concepts not necessary.
* [x] `split_by_regex.h`
  * No use of templates.
* [x] `split_path.h`
  * No use of templates.
* [x] `sqlite/Connection.h`
* [x] `sqlite/ConnectionFactory.h`
  * Concepts not necessary.
* [x] `sqlite/Exception.h`
  * No use of templates.
* [x] `sqlite/Ntuple.h`
  * Concepts not necessary.
* [x] `sqlite/Transaction.h`
  * No use of templates.
* [x] `sqlite/column.h`
* [x] `sqlite/create_table.h`
  * Concepts not necessary.
* [x] `sqlite/detail/DefaultDatabaseOpenPolicy.h`
  * No use of templates.
* [x] `sqlite/detail/bind_parameters.h`
* [x] `sqlite/detail/column_constraint.h`
  * No use of templates.
* [x] `sqlite/detail/convert.h`
  * Concepts not necessary.
* [x] `sqlite/detail/extract.h`
  * Concepts not necessary.
  * Redundant functional duplicate of `convert.h`?
* [x] `sqlite/detail/get_result.h`
  * Concepts not necessary as they would be satisified by construction
    (`filldata` only ever gets called with an `std::tuple`).
* [x] `sqlite/detail/normalize_statement.h`
  * No use of templates.
* [x] `sqlite/exec.h`
  * No use of templates.
* [x] `sqlite/helpers.h`
  * Concepts not necessary.
* [x] `sqlite/insert.h`
  * Concepts not necessary.
  * Minor drive-by improvement: `if` -> `if constexpr`.
* [x] `sqlite/query_result.h`
  * Concepts not necessary.
* [x] `sqlite/select.h`
* [x] `sqlite/statistics.h`
  * Concepts not necessary.
* [x] `trim.h`
  * No use of templates.
* [x] `value_ptr.h`
