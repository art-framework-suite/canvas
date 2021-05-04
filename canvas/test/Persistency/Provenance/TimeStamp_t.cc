#define BOOST_TEST_MODULE (TimeStamp_t)
#include "boost/test/unit_test.hpp"
#include "canvas/Persistency/Provenance/Timestamp.h"

using art::Timestamp;
using art::TimeValue_t;

BOOST_AUTO_TEST_SUITE(TimeStamp_t)

BOOST_AUTO_TEST_CASE(constructTest)
{
  constexpr TimeValue_t t = 2;
  constexpr Timestamp temp(t);

  static_assert(temp.value() == t);

  static_assert(Timestamp::invalidTimestamp() < Timestamp::beginOfTime());
  static_assert(Timestamp::beginOfTime() < Timestamp::endOfTime());
  static_assert(Timestamp::endOfTime().value() + 1 == 0);

  constexpr Timestamp db(0xdeadbeefbeefdead);

  static_assert(db.timeLow() == 0xbeefdead);
  static_assert(db.timeHigh() == 0xdeadbeef);
  static_assert(db.value() == 0xdeadbeefbeefdead);
}

BOOST_AUTO_TEST_CASE(comparisonTest)
{
  constexpr Timestamp small(1);
  constexpr Timestamp med(2);

  static_assert(small < med);
  static_assert(small <= med);
  static_assert(!(small == med));
  static_assert(small != med);
  static_assert(!(small > med));
  static_assert(!(small >= med));
}

BOOST_AUTO_TEST_SUITE_END()
