#ifndef canvas_Persistency_Common_detail_is_handle_h
#define canvas_Persistency_Common_detail_is_handle_h

#include <type_traits>

namespace art::detail {
  template <typename T, typename = void>
  struct is_handle : std::false_type {};

  template <typename T>
  struct is_handle<T, std::void_t<typename T::HandleTag>> : std::true_type {};

  template <typename T>
  constexpr bool is_handle_v = is_handle<T>::value;

  template <typename T, typename U>
  constexpr bool are_handles_v{is_handle_v<T> && is_handle_v<U>};
}
#endif /* canvas_Persistency_Common_detail_is_handle_h */

// Local Variables:
// mode: c++
// End:
