#define BOOST_TEST_MODULE (RangeSet_t)
#include "canvas/Persistency/Provenance/RangeSet.h"
#include "cetlib/quiet_unit_test.hpp"

#include <iostream>

using art::EventRange;
using art::RangeSet;

BOOST_AUTO_TEST_SUITE(RangeSet_t)

BOOST_AUTO_TEST_CASE(empty)
{
  auto rs = RangeSet::for_run(1);
  BOOST_CHECK(rs.has_disjoint_ranges());
  BOOST_CHECK(rs.is_sorted());
}

BOOST_AUTO_TEST_CASE(disjoint1)
{
  auto rs = RangeSet::for_run(1);
  rs.emplace_range(1,2,7);
  rs.emplace_range(1,9,14);
  BOOST_CHECK(rs.has_disjoint_ranges());
  BOOST_CHECK(rs.is_sorted());
}

BOOST_AUTO_TEST_CASE(disjoint2)
{
  auto rs1 = RangeSet::for_run(1);
  rs1.emplace_range(1,1,8);
  rs1.emplace_range(1,9,14);
  rs1.emplace_range(3,1,8);
  rs1.emplace_range(4,5,8);
  BOOST_CHECK(rs1.has_disjoint_ranges());
  BOOST_CHECK(rs1.is_sorted());

  auto rs2 = RangeSet::for_run(1);
  rs2.emplace_range(1,8,9);
  rs2.emplace_range(1,14,101);
  rs2.emplace_range(2,14,101);
  rs2.emplace_range(4,1,5);
  BOOST_CHECK(rs2.has_disjoint_ranges());
  BOOST_CHECK(rs2.is_sorted());

  BOOST_CHECK(art::are_disjoint(rs1,rs2));
}

BOOST_AUTO_TEST_CASE(collapsing1)
{
  auto rs = RangeSet::for_run(1);
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
  auto rs = RangeSet::for_run(1);
  rs.emplace_range(1,1,4);
  rs.emplace_range(1,1,11);
  rs.emplace_range(1,4,11);
  rs.emplace_range(2,1,4);
  rs.emplace_range(2,1,11);
  rs.emplace_range(2,4,11);
  BOOST_CHECK(rs.is_sorted());
  rs.collapse();

  auto ref = RangeSet::for_run(1);
  ref.emplace_range(1,1,11);
  ref.emplace_range(2,1,11);
  rs.collapse();
  BOOST_CHECK_EQUAL(rs, ref);

  std::string const& ref_string {"1:1[1,11)2[1,11)"};
  BOOST_CHECK_EQUAL(rs.to_compact_string(), ref_string);
}

BOOST_AUTO_TEST_CASE(sorting)
{
  auto rs = RangeSet::for_run(2);
  rs.emplace_range(1,1,3);
  rs.emplace_range(1,2,4);
  rs.emplace_range(1,4,8);
  rs.collapse();
  BOOST_CHECK_EQUAL(rs.ranges().size(), 2u);
  BOOST_CHECK(!rs.has_disjoint_ranges());
  rs.emplace_range(1,1,9);
  BOOST_CHECK(!rs.is_sorted());

  auto ref = RangeSet::for_run(2);
  ref.emplace_range(1,1,3);
  ref.emplace_range(1,2,8);
  ref.emplace_range(1,1,9);
  BOOST_CHECK_EQUAL(rs, ref);
}

BOOST_AUTO_TEST_CASE(merging1)
{
  // Ranges: [1,3) & [4,8)
  auto rs1 = RangeSet::for_run(2);
  rs1.emplace_range(1,1,3);
  rs1.emplace_range(1,4,8);
  rs1.collapse();
  BOOST_REQUIRE(rs1.has_disjoint_ranges());

  // Ranges: [3,4) & [8,11)
  auto rs2 = RangeSet::for_run(2);
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
  auto rs1 = RangeSet::for_run(2);
  rs1.emplace_range(1,1,3);
  rs1.emplace_range(1,4,8);
  rs1.collapse();
  BOOST_REQUIRE(rs1.has_disjoint_ranges());

  // Ranges: [1,7)
  auto rs2 = RangeSet::for_run(2);
  rs2.emplace_range(1,1,7);
  rs2.collapse();
  BOOST_REQUIRE(rs2.has_disjoint_ranges());

  BOOST_CHECK(!art::are_disjoint(rs1, rs2));
  rs1.merge(rs2);

  std::vector<EventRange> const ref_ranges {
    EventRange{1,1,3},
    EventRange{1,1,7},
    EventRange{1,4,8}
  };

  RangeSet const ref {2, ref_ranges};
  BOOST_CHECK_EQUAL(rs1, ref);
}

BOOST_AUTO_TEST_CASE(merging3)
{
  // Ranges: [1,3) & [4,8)
  auto rs1 = RangeSet::for_run(2);
  rs1.emplace_range(1,1,3);
  rs1.emplace_range(1,4,8);
  rs1.collapse();
  BOOST_REQUIRE(rs1.has_disjoint_ranges());

  // Ranges: [3,4)
  auto rs2 = RangeSet::for_run(2);
  rs2.emplace_range(1,3,4);
  rs2.collapse();
  BOOST_REQUIRE(rs2.has_disjoint_ranges());

  BOOST_REQUIRE(art::are_disjoint(rs1, rs2));
  rs1.merge(rs2);

  std::vector<EventRange> const ref_ranges {
    EventRange{1,1,8},
  };

  RangeSet const ref {2, ref_ranges};
  BOOST_CHECK_EQUAL(rs1, ref);
}

BOOST_AUTO_TEST_SUITE_END()
