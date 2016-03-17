#define BOOST_TEST_MODULE (RangeSet_t)
#include "canvas/Persistency/Provenance/RangeSet.h"
#include "cetlib/quiet_unit_test.hpp"

#include <iostream>

using art::EventRange;
using art::RangeSet;

BOOST_AUTO_TEST_SUITE(RangeSet_t)

BOOST_AUTO_TEST_CASE(empty)
{
  RangeSet rs {1};
  BOOST_CHECK(rs.has_disjoint_ranges());
  BOOST_CHECK(rs.is_sorted());
}

BOOST_AUTO_TEST_CASE(disjoint)
{
  RangeSet rs {1};
  rs.emplace_range(1,2,7);
  rs.emplace_range(1,9,14);
  BOOST_CHECK(rs.has_disjoint_ranges());
  BOOST_CHECK(rs.is_sorted());
}

BOOST_AUTO_TEST_CASE(collapsing1)
{
  RangeSet rs {1};
  rs.emplace_range(1,1,4);
  rs.emplace_range(1,4,6);
  rs.emplace_range(1,6,11);
  rs.emplace_range(1,11,101);
  BOOST_CHECK_EQUAL(rs.ranges().size(), 4u);
  rs.collapse();
  BOOST_REQUIRE_EQUAL(rs.ranges().size(), 1u);
  BOOST_CHECK_EQUAL(rs.front().begin(), 1);
  BOOST_CHECK_EQUAL(rs.front().end(), 101);
  BOOST_CHECK(rs.has_disjoint_ranges());
}

BOOST_AUTO_TEST_CASE(collapsing2)
{
  RangeSet rs {1};
  rs.emplace_range(1,1,4);
  rs.emplace_range(1,1,11);
  rs.emplace_range(1,4,11);
  rs.emplace_range(2,1,4);
  rs.emplace_range(2,1,11);
  rs.emplace_range(2,4,11);
  BOOST_CHECK(rs.is_sorted());
  rs.collapse();

  RangeSet ref {1};
  ref.emplace_range(1,1,11);
  ref.emplace_range(2,1,11);
  rs.collapse();
  BOOST_CHECK_EQUAL(rs, ref);

  std::string const& ref_string {"1:1[1,11)2[1,11)"};
  BOOST_CHECK_EQUAL(rs.to_compact_string(), ref_string);
}

BOOST_AUTO_TEST_CASE(sorting)
{
  RangeSet rs {2};
  rs.emplace_range(1,1,3);
  rs.emplace_range(1,2,4);
  rs.emplace_range(1,4,8);
  rs.collapse();
  BOOST_CHECK_EQUAL(rs.ranges().size(), 2u);
  BOOST_CHECK(!rs.has_disjoint_ranges());
  rs.emplace_range(1,1,9);
  BOOST_CHECK(!rs.is_sorted());

  RangeSet ref {2};
  ref.emplace_range(1,1,3);
  ref.emplace_range(1,2,8);
  ref.emplace_range(1,1,9);
  BOOST_CHECK_EQUAL(rs, ref);
}

BOOST_AUTO_TEST_CASE(merging1)
{
  // Ranges: [1,3) & [4,8)
  RangeSet rs1 {2};
  rs1.emplace_range(1,1,3);
  rs1.emplace_range(1,4,8);
  rs1.collapse();
  BOOST_REQUIRE(rs1.has_disjoint_ranges());

  // Ranges: [3,4) & [8,11)
  RangeSet rs2 {2};
  rs2.emplace_range(1,3,4);
  rs2.emplace_range(1,8,11);
  rs2.collapse();
  BOOST_REQUIRE(rs2.has_disjoint_ranges());

  rs1.merge(rs2); // Ranges: [1,3), [3,4), [4,8) & [8,11)
                  // collapse to: [1,11)
  BOOST_CHECK_EQUAL(rs1.ranges().size(), 1u);
  BOOST_CHECK_EQUAL(rs1.front().begin(), 1);
  BOOST_CHECK_EQUAL(rs1.front().end(), 11);
}

BOOST_AUTO_TEST_CASE(merging2)
{
  // Ranges: [1,3) & [4,8)
  RangeSet rs1 {2};
  rs1.emplace_range(1,1,3);
  rs1.emplace_range(1,4,8);
  rs1.collapse();
  BOOST_REQUIRE(rs1.has_disjoint_ranges());

  // Ranges: [1,7)
  RangeSet rs2 {2};
  rs2.emplace_range(1,1,7);
  rs2.collapse();
  BOOST_REQUIRE(rs2.has_disjoint_ranges());

  rs1.merge(rs2);
  BOOST_CHECK_EQUAL(rs1.ranges().size(), 3u);

  std::vector<EventRange> const ref_ranges {
    EventRange{1,1,3},
    EventRange{1,1,7},
    EventRange{1,4,8}
  };

  RangeSet const ref {2, ref_ranges};
  BOOST_CHECK_EQUAL(rs1, ref);
}

BOOST_AUTO_TEST_SUITE_END()
