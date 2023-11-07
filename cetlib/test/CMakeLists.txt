include(CetTest)
cet_enable_asserts()
cet_test_env("LC_ALL=en_US.utf8")

set(THREADS_PREFER_PTHREAD_FLAG TRUE)

find_package(Catch2 REQUIRED)

# Put everything in a different export set.
cet_register_export_set(SET_NAME Testing NAMESPACE cetlib_test SET_DEFAULT)

add_subdirectory(sqlite)

cet_test(bit_test)
cet_test(value_ptr_test_2)
cet_test(value_ptr_test_3)
cet_test(value_ptr_test_4)
cet_test(value_ptr_test_5)

# Tests requiring Boost's unit test framework.
foreach (test IN ITEMS
    canonical_string_test
    column_width_test
    container_algs_test
    exempt_ptr_test
    includer_test
    is_absolute_filepath_t
    lpad_test
    map_vector_test
    maybe_ref_test
    ostream_handle_test
    pow_test
    rpad_test
    search_path_test_3
    shlib_utils_t
    simple_stats_t
    split_by_regex_test
    split_test
    trim_test
    value_ptr_test
)
  cet_test(${test} USE_BOOST_UNIT)
endforeach()

# Other tests.
foreach (test IN ITEMS
    base_converter_test
    filesystem_test
    include_test
    MD5Digest_test
    nybbler_test
    pow_constexpr_test
    registry_test
    registry_via_id_test
    registry_via_id_test_2
    sha1_test
)
  cet_test(${test} LIBRARIES)
endforeach()

cet_test(cpu_timer_test USE_BOOST_UNIT
  TEST_PROPERTIES RUN_SERIAL true
  OPTIONAL_GROUPS LOAD_SENSITIVE)
cet_test(filepath_maker_test USE_BOOST_UNIT
  TEST_PROPERTIES
  ENVIRONMENT "CURRENT_DIR=${CMAKE_CURRENT_SOURCE_DIR};\
FILEPATH_MAKER_TEST_FILES=${CMAKE_CURRENT_SOURCE_DIR}/filepath_maker-files"
  TEST_WORKDIR "${CMAKE_CURRENT_SOURCE_DIR}"
  LIBRARIES PRIVATE cetlib::cetlib)

cet_test(inc-expand_test.sh PREBUILT DEPENDENCIES inc-expand)

cet_make_exec(NAME search_path_test_2 NO_INSTALL)
cet_test(search_path_test_2.sh PREBUILT DEPENDENCIES search_path_test_2)

cet_test(sha1_test_2 SOURCE sha1_test_2.cc sha1.cpp)
cet_test(sha1_test_performance SOURCE sha1_test_performance.cc sha1.cpp)

cet_test(split_search_path_t USE_CATCH2_MAIN)

# Add cetlib::cetlib as a dependency where necessary.
foreach (target IN ITEMS
    MD5Digest_test
    base_converter_test
    canonical_string_test
    column_width_test
    container_algs_test
    cpu_timer_test
    exempt_ptr_test
    filesystem_test
    include_test
    includer_test
    is_absolute_filepath_t
    lpad_test
    map_vector_test
    maybe_ref_test
    nybbler_test
    ostream_handle_test
    pow_constexpr_test
    pow_test
    registry_test
    registry_via_id_test
    registry_via_id_test_2
    rpad_test
    search_path_test_2
    search_path_test_3
    sha1_test
    sha1_test_2
    sha1_test_performance
    shlib_utils_t
    simple_stats_t
    split_by_regex_test
    split_search_path_t
    split_test
    trim_test
    value_ptr_test)
  target_link_libraries(${target} PRIVATE cetlib::cetlib)
endforeach()

# Take advantage of C++23 for split_search_path_t if we can.
if (cxx_std_23 IN_LIST CMAKE_CXX_COMPILE_FEATURES)
  target_compile_features(split_search_path_t PRIVATE cxx_std_23)
endif()

####################################
# Test plugin machinery.
cet_test(plugin_search_path_t USE_CATCH2_MAIN LIBRARIES PRIVATE cetlib::cetlib)

cet_make_library(LIBRARY_NAME cetlib_test_fakePlugin SOURCE moduleType.cc NO_INSTALL)
cet_make_library(LIBRARY_NAME cetlib_test_TestPluginBase SOURCE TestPluginBase.cc NO_INSTALL
  LIBRARIES PRIVATE art_plugin_support::support_macros)

include(BasicPlugin)
basic_plugin(TestPlugin "plugin" NO_INSTALL
  LIBRARIES PRIVATE cetlib_test::TestPluginBase)

# Use default Plugin lookup
cet_test(PluginFactory_t USE_BOOST_UNIT
  LIBRARIES PRIVATE cetlib_test::TestPluginBase cetlib::cetlib)

# Use custom Plugin lookup
cet_test(PluginFactoryCustomSearchPath_t USE_BOOST_UNIT
  SOURCE PluginFactory_t.cc
  LIBRARIES PRIVATE cetlib_test::TestPluginBase cetlib::cetlib
  TEST_PROPERTIES ENVIRONMENT
  PLUGIN_FACTORY_SEARCH_PATH=$<TARGET_FILE_DIR:cetlib_test::TestPlugin_plugin>)
target_compile_definitions(PluginFactoryCustomSearchPath_t PRIVATE PLUGIN_FACTORY_SEARCH_PATH=1)

function(test_library LIBSPEC)
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${LIBSPEC}.cc
    COMMAND ${CMAKE_COMMAND}
    -DSRC_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
    -DBIN_DIR="${CMAKE_CURRENT_BINARY_DIR}"
    -DLIBSPEC="${LIBSPEC}"
    -P ${CMAKE_CURRENT_SOURCE_DIR}/configureLibraryManagerTestFunc.cmake
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/configureLibraryManagerTestFunc.cmake
    ${CMAKE_CURRENT_SOURCE_DIR}/LibraryManagerTestFunc.cc.in)
  add_library(${LIBSPEC}_cetlibtest SHARED ${CMAKE_CURRENT_BINARY_DIR}/${LIBSPEC}.cc)
  target_link_libraries(${LIBSPEC}_cetlibtest PRIVATE cetlib::cetlib)
endfunction()

test_library(1_1_1)
test_library(1_1_2)
test_library(1_1_3)
test_library(1_2_3)
test_library(2_1_5)

# Use default library search path
cet_test(LibraryManager_t USE_BOOST_UNIT
  LIBRARIES PRIVATE
    cetlib::cetlib
    ${CMAKE_DL_LIBS}
)

# Use custom library search path
cet_test(LibraryManagerCustomSearchPath_t USE_BOOST_UNIT
  SOURCE LibraryManager_t.cc
    LIBRARIES PRIVATE
      cetlib::cetlib
      ${CMAKE_DL_LIBS}
  TEST_PROPERTIES ENVIRONMENT LIBRARY_MANAGER_SEARCH_PATH=$<TARGET_FILE_DIR:1_1_1_cetlibtest>
)
target_compile_definitions(LibraryManagerCustomSearchPath_t
  PRIVATE LIBRARY_MANAGER_SEARCH_PATH=1)

cet_test(replace_all_test USE_BOOST_UNIT LIBRARIES PRIVATE cetlib::cetlib)
cet_test(regex_t USE_BOOST_UNIT LIBRARIES PRIVATE cetlib::cetlib
  TEST_WORKDIR ${CMAKE_CURRENT_SOURCE_DIR})
cet_test(regex_standalone_t SOURCE regex_t.cc
  TEST_WORKDIR ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(regex_standalone_t PRIVATE STANDALONE_TEST)

########################################################################
# Catch2 unit tests

cet_test(getenv_test USE_CATCH2_MAIN LIBRARIES PRIVATE cetlib::cetlib)
cet_test(os_libpath_t USE_CATCH2_MAIN)
cet_test(search_path_test USE_CATCH2_MAIN TEST_PROPERTIES ENVIRONMENT xyzzy=""
         LIBRARIES PRIVATE cetlib::cetlib)
cet_test(canonical_number_test USE_CATCH2_MAIN
         LIBRARIES PRIVATE cetlib::cetlib)
cet_test(crc32_test USE_CATCH2_MAIN
         SOURCE crc32_test.cc CRC32Calculator.cc
         TEST_ARGS -s
         LIBRARIES PRIVATE cetlib::cetlib)

# Use ParseAndAddCatchTests to generate a test for each test case.
cet_make_exec(NAME hypot_test USE_CATCH2_MAIN NO_INSTALL)
include(ParseAndAddCatchTests)
set(AdditionalCatchParameters -s)
ParseAndAddCatchTests(hypot_test)

foreach(compile_test IN ITEMS
    bad_hard_cast_t bad_copy_all_t bad_for_all_container_t bad_for_all_func_t bad_pow_t bad_search_path_t bad_lib_manager_t bad_bit_manipulation_t)
  cet_test(${compile_test} COMPILE_ONLY TEST_PROPERTIES WILL_FAIL TRUE)
endforeach()

foreach(compile_test IN ITEMS
    bad_value_ptr_t bad_value_ptr_t_2 bad_value_ptr_t_3)
  cet_test(${compile_test} COMPILE_ONLY)
endforeach()
if (CMAKE_CXX_STANDARD GREATER_EQUAL 20)
  set_property(TEST bad_value_ptr_t PROPERTY PASS_REGULAR_EXPRESSION
    "is_compatible_v<E2>.*evaluated to ")
  set_tests_properties(bad_value_ptr_t_2 bad_value_ptr_t_3
    PROPERTIES PASS_REGULAR_EXPRESSION
    "!\\(WouldSlice<Element, Cloner, E2>\\).*evaluated to ")
else()
  set_tests_properties(bad_value_ptr_t bad_value_ptr_t_2 bad_value_ptr_t_3
    PROPERTIES WILL_FAIL TRUE)
endif()

cet_test(bad_hypot_t COMPILE_ONLY)
if (CMAKE_CXX_STANDARD GREATER_EQUAL 20)
  set_property(TEST bad_hypot_t PROPERTY PASS_REGULAR_EXPRESSION
    "is_arithmetic<T>.*evaluated to ")
else()
  set_tests_properties(bad_hypot_t
    PROPERTIES WILL_FAIL TRUE)
endif()

foreach (compile_test IN ITEMS
    bad_reg_via_id_t bad_reg_via_id_t_2)
  cet_test(${compile_test} COMPILE_ONLY LIBRARIES PRIVATE cetlib_except::cetlib_except)
endforeach()
if (CMAKE_CXX_STANDARD GREATER_EQUAL 20)
  set_property(TARGET bad_reg_via_id_t bad_reg_via_id_t_2 APPEND PROPERTY
    COMPILE_OPTIONS "$<$<COMPILE_LANGUAGE:CXX>:-fconcepts-diagnostics-depth=2>")
  set_property(TEST bad_reg_via_id_t PROPERTY
    PASS_REGULAR_EXPRESSION "error: ‘struct thing’ has no member named ‘id’")
  set_property(TEST bad_reg_via_id_t_2 PROPERTY
    PASS_REGULAR_EXPRESSION "note: the expression ‘is_same_v<_Tp, _Up> \\[with _Tp = float; _Up = char\\]’ evaluated to ‘false’")
else()
  set_tests_properties(bad_reg_via_id_t bad_reg_via_id_t_2
    PROPERTIES WILL_FAIL TRUE)
endif()
