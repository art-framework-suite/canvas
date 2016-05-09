#ifndef canvas_Utilities_detail_metaprogramming_h
#define canvas_Utilities_detail_metaprogramming_h
// Common metaprogramming utilities.

namespace art {
  namespace detail {
    using no_tag  = char(&)[1]; // type indicating FALSE
    using yes_tag = char(&)[2]; // type indicating TRUE

    // SFINAE magic for determining if type exists
    template <class T, class R = void>
    struct enable_if_type { using type = R; };

    template <class T>
    using enable_if_type_t = typename enable_if_type<T>::type;
  }
}
#endif /* canvas_Utilities_detail_metaprogramming_h */

// Local Variables:
// mode: c++
// End:
