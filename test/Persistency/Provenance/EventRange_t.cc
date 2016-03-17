#define BOOST_TEST_MODULE (EventRange_t)
#include "canvas/Persistency/Provenance/EventRange.h"
#include "cetlib/quiet_unit_test.hpp"

#include <iostream>

using art::EventRange;

BOOST_AUTO_TEST_SUITE(EventRange_t)

BOOST_AUTO_TEST_CASE(merge1)
{
  EventRange er1 {1,2,4};
  EventRange er2 {1,3,6};
  BOOST_CHECK(!er1.merge(er2));
  BOOST_CHECK(!er1.is_disjoint(er2));
}

BOOST_AUTO_TEST_CASE(merge2)
{
  EventRange er1 {1,2,4};
  EventRange const er2 {1,4,6};
  BOOST_CHECK(er1.is_adjacent(er2));
  BOOST_CHECK(er1.is_disjoint(er2));
  BOOST_CHECK(er1.merge(er2));

  EventRange const ref {1,2,6};
  BOOST_CHECK_EQUAL(er1, ref);
  BOOST_CHECK(er1.contains(2));
  BOOST_CHECK(er1.contains(4));
  BOOST_CHECK(!er1.contains(6));
}

BOOST_AUTO_TEST_CASE(lessThan)
{
  EventRange const er0 {1,1,6};
  EventRange const er1 {1,2,4};
  EventRange const er2 {1,2,5};
  EventRange const er3 {2,2,5};
  EventRange const er4 {2,6,9};
  BOOST_CHECK(er0 < er1);
  BOOST_CHECK(er1 < er2);
  BOOST_CHECK(er2 < er3);
  BOOST_CHECK(er0.is_disjoint(er3));
  BOOST_CHECK(!er0.is_disjoint(er1));
  BOOST_CHECK(er3.is_disjoint(er4));
}

BOOST_AUTO_TEST_SUITE_END()
