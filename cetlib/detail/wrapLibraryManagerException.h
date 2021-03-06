#ifndef cetlib_detail_wrapLibraryManagerException_h
#define cetlib_detail_wrapLibraryManagerException_h

#include <string>

namespace cet {
  class exception;
}

namespace cet::detail {
  void wrapLibraryManagerException(cet::exception const& e,
                                   std::string const& item_type,
                                   std::string const& libspec,
                                   std::string const& release);
}

#endif /* cetlib_detail_wrapLibraryManagerException_h */

// Local Variables:
// mode: c++
// End:
