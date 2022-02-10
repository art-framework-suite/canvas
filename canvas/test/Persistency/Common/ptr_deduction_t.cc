#include "canvas/Persistency/Common/Ptr.h"
#include "cetlib/map_vector.h"

#include <vector>

using namespace art;

namespace {
  template <typename P>
  class FakeHandle {
  public:
    using element_type = P;
    // No need to define member functions as they're only used for type
    // deduction.
    ProductID id() const;
    P const* product() const;
    EDProductGetter const* productGetter() const;
  };

  template <typename P, typename T, typename = void>
  struct handle_deduces_to_t : std::false_type {};

  template <typename P, typename T>
  struct handle_deduces_to_t<
    P,
    T,
    std::enable_if_t<std::is_same_v<decltype(Ptr{FakeHandle<P>{}, 2}), T>>>
    : std::true_type {};

  template <typename P, typename T>
  constexpr bool handle_deduces_to{handle_deduces_to_t<P, T>::value};
}

int
main()
{
  static_assert(handle_deduces_to<std::vector<int>, Ptr<int>>);
  static_assert(handle_deduces_to<std::list<int>, Ptr<int>>);

  using int_to_double = std::map<int, double>;
  static_assert(
    handle_deduces_to<int_to_double, Ptr<int_to_double::value_type>>);

  // CTAD is disabled for cet::map_vector.
  static_assert(
    not handle_deduces_to<cet::map_vector<double>,
                          Ptr<cet::map_vector<double>::mapped_type>>);
  static_assert(
    not handle_deduces_to<cet::map_vector<double>,
                          Ptr<cet::map_vector<double>::value_type>>);
}
