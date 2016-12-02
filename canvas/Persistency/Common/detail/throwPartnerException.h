#ifndef LAKSHFGJAGJAF
#define LAKSHFGJAGJAF

#include <typeinfo>

namespace art {
  namespace detail {
    [[noreturn]]
      void throwPartnerException(std::type_info const & generator,
                                 std::type_info const & wanted_wrapper_type);
  }
}
#endif
