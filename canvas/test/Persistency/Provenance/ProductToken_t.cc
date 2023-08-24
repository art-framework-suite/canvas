#define BOOST_TEST_MODULE (ProductToken_t)
#include "boost/test/unit_test.hpp"
#include "canvas/Persistency/Provenance/ProductToken.h"
#include "canvas/Utilities/InputTag.h"

#include <sstream>
#include <string>

BOOST_AUTO_TEST_SUITE(ProductToken_t)

BOOST_AUTO_TEST_CASE(streamingProductToken)
{
  art::InputTag tag_x{"x"};
  std::ostringstream os1;
  os1 << tag_x;
  std::string expected = os1.str();

  art::ProductToken<int> token_x{tag_x};
  std::ostringstream os2;
  os2 << token_x;
  std::string result = os2.str();
  BOOST_TEST(result == expected);
}

BOOST_AUTO_TEST_CASE(streamingViewToken)
{
  art::InputTag tag_x{"x"};
  std::ostringstream os1;
  os1 << tag_x;
  std::string expected = os1.str();

  art::ViewToken<int> token_x{tag_x};
  std::ostringstream os2;
  os2 << token_x;
  std::string result = os2.str();
  BOOST_TEST(result == expected);
}

BOOST_AUTO_TEST_SUITE_END()
