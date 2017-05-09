#define BOOST_TEST_MODULE (TypeIDAssns_t)
#include "cetlib/quiet_unit_test.hpp"

#include <iostream>
#include <typeinfo>
#include <string>
#include <vector>

#include "canvas/Persistency/Common/Assns.h"
#include "canvas/Utilities/TypeID.h"

namespace {
  // Declared in anonymous namespace so that the friendly class-name
  // has "(anonymous namespace)::" in its name.  This tests the
  // friendly-name calculation with parentheses contained in the
  // demangled name.
  struct A{};
  struct B{};
  struct D{};
}


using namespace art;

BOOST_AUTO_TEST_SUITE(TypeID_t)

BOOST_AUTO_TEST_CASE(assns)
{
  TypeID const abd{typeid(Assns<A, B, D>)};
  TypeID const bad{typeid(Assns<B, A, D>)};
  BOOST_CHECK(abd != bad);
  BOOST_CHECK_EQUAL(abd.friendlyClassName(), bad.friendlyClassName());
}

BOOST_AUTO_TEST_SUITE_END()
