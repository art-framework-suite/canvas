#define BOOST_TEST_MODULE (Tests setPtr customization)
#include "boost/test/unit_test.hpp"

#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/Wrapper.h"

#include <type_traits>
#include <vector>

namespace custom {
  template <typename T>
  struct collection {
    using impl = std::vector<T>;
    using const_iterator = typename impl::const_iterator;
    using value_type = typename impl::value_type;
    const_iterator
    cbegin() const
    {
      return v.cbegin();
    }
    impl v;
  };
}

namespace {
  struct customization_honored {};
  auto always_true = [](customization_honored) { return true; };
}

/* namespace art {
  template <has_setPtr T>
  struct custom::collection<T> : std::true_type {};
}*/

namespace custom {
  template <class T>
  [[noreturn]] void
  setPtr(collection<T> const&,
         std::type_info const&,
         unsigned long,
         void const*&)
  {
    throw customization_honored{};
  }

  template <typename T>
  [[noreturn]] void
  getElementAddresses(collection<T> const&,
                      std::type_info const&,
                      std::vector<unsigned long> const&,
                      std::vector<void const*>&)
  {
    throw customization_honored{};
  }
}

BOOST_AUTO_TEST_SUITE(ptr_customizations_t)

BOOST_AUTO_TEST_CASE(setPtr_t)
{
  art::Wrapper<custom::collection<int>> w;
  BOOST_CHECK_EXCEPTION(
    w.getElementAddress(typeid(int), {}), customization_honored, always_true);
}

BOOST_AUTO_TEST_CASE(getElementAddresses_t)
{
  art::Wrapper<custom::collection<int>> w;
  BOOST_CHECK_EXCEPTION(
    w.getElementAddresses(typeid(int), {}), customization_honored, always_true);
}

BOOST_AUTO_TEST_SUITE_END()
