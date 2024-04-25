#ifndef cetlib_ProvideFilePathMacro_h
#define cetlib_ProvideFilePathMacro_h

// =====================================================================
//
// Macro that creates file-path provider.
//
// =====================================================================

#include "cetlib/compiler_macros.h"
#include "cetlib/detail/provide_file_path.h"

#include <source_location>
#include <string>

#define CET_PROVIDE_FILE_PATH()                                                \
  EXTERN_C_FUNC_DECLARE_START                                                  \
  std::string get_source_location()                                            \
  {                                                                            \
    return cet::detail::provide_file_path(                                     \
      std::source_location::current().file_name());                            \
  }                                                                            \
  EXTERN_C_FUNC_DECLARE_END

#endif /* cetlib_ProvideFilePathMacro_h */

// Local Variables:
// mode: c++
// End:
