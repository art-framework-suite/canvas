#define BOOST_TEST_MODULE (Tests of Wrapper interface)
#include "cetlib/quiet_unit_test.hpp"

#include "canvas/Persistency/Common/Sampled.h"
#include "canvas/Persistency/Common/Wrapper.h"
#include "canvas/Utilities/InputTag.h"

using namespace art;

namespace {
  InputTag const invalid{};

  template <typename T>
  std::unique_ptr<EDProduct>
  make_product(T&& t)
  {
    auto product = std::make_unique<T>(std::forward<T>(t));
    return std::make_unique<Wrapper<T>>(std::move(product));
  }
}

BOOST_AUTO_TEST_SUITE(wrapper_t)

BOOST_AUTO_TEST_CASE(sampled_product)
{
  Wrapper<int> w;
  auto edp = w.createEmptySampledProduct(invalid);
  BOOST_TEST_REQUIRE(edp.get());
  BOOST_TEST(edp->isPresent());

  auto const& sampled = static_cast<Wrapper<Sampled<int>> const&>(*edp);
  BOOST_TEST(sampled->empty());
}

BOOST_AUTO_TEST_CASE(no_sampled_product_recursion)
{
  Wrapper<Sampled<int>> w;
  BOOST_CHECK_EXCEPTION(
    w.createEmptySampledProduct(invalid), Exception, [](auto const& e) {
      return e.categoryCode() == errors::LogicError and
             e.explain_self().find("recursion is not allowed") !=
               std::string::npos;
    });
}

BOOST_AUTO_TEST_CASE(print_size)
{
  auto int_product = make_product(3);
  BOOST_TEST(int_product->productSize() == "-");

  auto map_product = make_product<std::map<int, double>>({{1, 1.5}, {2, 3.}});
  BOOST_TEST(map_product->productSize() == "2");

  struct CustomTypeWithSize {
    size_t
    size() const noexcept
    {
      return 3;
    }
  };
  auto custom_product = make_product<CustomTypeWithSize>({});
  BOOST_TEST(custom_product->productSize() == "3");
}

BOOST_AUTO_TEST_SUITE_END()
