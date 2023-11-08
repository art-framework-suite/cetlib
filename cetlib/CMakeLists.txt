# ======================================================================
#
# Build/install script
#
# ======================================================================

include(CetMake)

find_package(cetlib_except REQUIRED EXPORT)

####################################
# Libraries.
cet_make_library(LIBRARY_NAME container_algorithms INTERFACE
  SOURCE container_algorithms.h)

cet_make_library(LIBRARY_NAME pow INTERFACE
  SOURCE pow.h
  LIBRARIES INTERFACE cetlib_except::cetlib_except
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/shlib_utils.cc.in
  ${CMAKE_CURRENT_BINARY_DIR}/shlib_utils.cc @ONLY
)

cet_make_library(HEADERS_TARGET
  SOURCE
    ${CMAKE_CURRENT_BINARY_DIR}/shlib_utils.cc
    MD5Digest.cc
    BasicPluginFactory.cc
    LibraryManager.cc
    PluginFactory.cc
    base_converter.cc
    canonical_number.cc
    canonical_string.cc
    column_width.cc
    cpu_timer.cc
    crc32.cc
    detail/ostream_handle_impl.cc
    detail/provide_file_path.cc
    detail/plugin_search_path.cc
    detail/wrapLibraryManagerException.cc
    filepath_maker.cc
    filesystem.cc
    getenv.cc
    include.cc
    includer.cc
    loadable_libraries.cc
    lpad.cc
    nybbler.cc
    replace_all.cc
    rpad.cc
    search_path.cc
    sha1.cc
    simple_stats.cc
    split_path.cc
    split_by_regex.cc
  LIBRARIES
    PUBLIC
      cetlib_except::cetlib_except
      cetlib::container_algorithms
    PRIVATE
      ${CMAKE_DL_LIBS}
)

# find openSSL
# when building on macOS, use CommonCrypto
if(NOT APPLE)
  find_package(OpenSSL REQUIRED EXPORT)
  target_link_libraries(cetlib PUBLIC OpenSSL::Crypto)
  if(OPENSSL_VERSION VERSION_GREATER_EQUAL 3.0)
    target_compile_definitions(cetlib PUBLIC OPENSSL_API_COMPAT=0x10100000L)
  endif()
endif()

cet_make_library(LIBRARY_NAME compiler_macros INTERFACE
  SOURCE compiler_macros.h)

cet_register_export_set(SET_NAME PluginSupport NAMESPACE art_plugin_support)

cet_make_library(LIBRARY_NAME support_macros INTERFACE
  EXPORT_SET PluginSupport
  SOURCE
    ProvideFilePathMacro.h
    ProvideMakePluginMacros.h
    PluginTypeDeducer.h
  LIBRARIES INTERFACE
    cetlib::compiler_macros
)

find_package(Boost COMPONENTS program_options REQUIRED EXPORT)
cet_make_library(LIBRARY_NAME cetlib_parsed_program_options
  SOURCE parsed_program_options.cc
  LIBRARIES
    PUBLIC Boost::program_options
    PRIVATE cetlib_except::cetlib_except
)
####################################

# Executables.
cet_make_exec(NAME inc-expand LIBRARIES PRIVATE cetlib::cetlib)
cet_make_exec(NAME get-accessible-paths LIBRARIES PRIVATE cetlib::cetlib)

# Headers and source.
install_headers(SUBDIRS detail)
install_source(SUBDIRS detail)

# SQLite functionality.
add_subdirectory(sqlite)

# Tests
if (BUILD_TESTING)
  add_subdirectory(test)
endif()
