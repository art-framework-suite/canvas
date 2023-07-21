#ifndef canvas_Utilities_ensurePointer_h
#define canvas_Utilities_ensurePointer_h

// Ensure we can get a desired pointer type from a compatible iterator.

// In order to allow new iterator value types (e.g. smart pointers) to
// be converted to pointers, specialize art::detail::EnsurePointer.

// See canvas/Persistency/Common/Ptr.h for an example of this. Also see
// test/Utilities/ensurePointer.h for testing coverage illustration.

#include "canvas/Utilities/Exception.h"
#include "cetlib_except/demangle.h"

#include <concepts>
#include <iterator>
#include <type_traits>

namespace art {

  template <typename WANTED_POINTER, typename InputIterator>
  WANTED_POINTER ensurePointer(InputIterator it);

  template <typename TO, typename FROM>
  concept are_cv_compatible_b =
    std::same_as<std::remove_cv_t<std::remove_pointer_t<TO>>,
                 std::remove_cv_t<std::remove_pointer_t<FROM>>> ||
    std::derived_from<std::remove_cv_t<std::remove_pointer_t<FROM>>,
                      std::remove_cv_t<std::remove_pointer_t<TO>>>;

  namespace detail {

    template <typename TO, typename FROM>
      requires art::are_cv_compatible_b<TO, FROM>
    std::add_pointer_t<std::remove_pointer_t<TO>>
    addr(FROM& from)
    {
      return &from;
    }

    template <typename TO, typename FROM>
      requires(!art::are_cv_compatible_b<TO, FROM> &&
               art::are_cv_compatible_b<FROM, TO>)
    std::add_pointer_t<std::remove_pointer_t<TO>> addr(FROM& from)
    {
      return &dynamic_cast<
        std::add_lvalue_reference_t<std::remove_pointer_t<TO>>>(from);
    }

    template <typename TO, typename FROM>
    struct EnsurePointer {
      constexpr TO
      operator()(FROM& from) const
      {
        return addr<TO>(from);
      }
      constexpr TO
      operator()(FROM const& from) const
      {
        return addr<TO>(from);
      }
    };

    template <typename TO, typename PFROM>
    struct EnsurePointer<TO, PFROM*> {
      constexpr TO
      operator()(PFROM* from) const
      {
        return addr<TO>(*from);
      }
    };
  }
}

template <typename WANTED_POINTER, typename InputIterator>
inline WANTED_POINTER
art::ensurePointer(InputIterator it)
try {
  static_assert(std::is_pointer_v<WANTED_POINTER>,
                "Supplied template argument is not a pointer.");
  return detail::EnsurePointer<
    WANTED_POINTER,
    typename std::iterator_traits<InputIterator>::value_type>{}(*it);
}
catch (std::bad_cast&) {
  throw Exception(errors::LogicError, "ensurePointer")
    << "Iterator value type "
    << cet::demangle_symbol(
         typeid(typename std::iterator_traits<InputIterator>::value_type)
           .name())
    << " and wanted pointer type "
    << cet::demangle_symbol(typeid(WANTED_POINTER).name())
    << " are incompatible.\n";
}
#endif /* canvas_Utilities_ensurePointer_h */

// Local Variables:
// mode: c++
// End:
