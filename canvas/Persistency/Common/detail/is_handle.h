#ifndef canvas_Persistency_Common_detail_is_handle_h
#define canvas_Persistency_Common_detail_is_handle_h

#include "canvas/Utilities/detail/metaprogramming.h"

#include <type_traits>

namespace art {
  namespace detail {
    template <class T, class Enable = void>
    struct is_handle : std::false_type {};

    template <class T>
    struct is_handle<T, typename enable_if_type<typename T::HandleTag>::type> : std::true_type { };
  }
}
#endif /* canvas_Persistency_Common_detail_is_handle_h */

// Local Variables:
// mode: c++
// End:
