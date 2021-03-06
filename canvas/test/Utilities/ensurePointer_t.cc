#define BOOST_TEST_MODULE(ensurePointer_t)
#include "boost/test/unit_test.hpp"

#include "canvas/Utilities/ensurePointer.h"

#include <vector>

struct A {
  virtual ~A() {}
};
struct B : A {};

using avec_t = std::vector<A>;
using apvec_t = std::vector<A*>;
using acpvec_t = std::vector<A const*>;
using bvec_t = std::vector<B>;
using bpvec_t = std::vector<B*>;
using bcpvec_t = std::vector<B const*>;
using ivec_t = std::vector<int>;
using ipvec_t = std::vector<int*>;
using icpvec_t = std::vector<int const*>;

BOOST_AUTO_TEST_SUITE(ensurePointer_t)

// Check basic type handling.
BOOST_AUTO_TEST_CASE(basic_types)
{
  ivec_t i(1);
  BOOST_CHECK_NO_THROW(art::ensurePointer<int*>(i.begin()));
  ivec_t const ic(1);
  BOOST_CHECK_NO_THROW(art::ensurePointer<int const*>(ic.begin()));
  ipvec_t ip(1);
  BOOST_CHECK_NO_THROW(art::ensurePointer<int*>(ip.begin()));
  icpvec_t icp(1);
  BOOST_CHECK_NO_THROW(art::ensurePointer<int const*>(icp.begin()));
}

// No constness.
BOOST_AUTO_TEST_CASE(a_pointer_from_a)
{
  avec_t a(1);
  BOOST_CHECK_NO_THROW(art::ensurePointer<A*>(a.begin()));
}

BOOST_AUTO_TEST_CASE(a_pointer_from_a_pointer)
{
  apvec_t a(1);
  BOOST_CHECK_NO_THROW(art::ensurePointer<A*>(a.begin()));
}

BOOST_AUTO_TEST_CASE(a_pointer_from_b)
{
  bvec_t b(1);
  BOOST_CHECK_NO_THROW(art::ensurePointer<A*>(b.begin()));
}

BOOST_AUTO_TEST_CASE(a_pointer_from_b_pointer)
{
  bpvec_t b(1);
  BOOST_CHECK_NO_THROW(art::ensurePointer<A*>(b.begin()));
}

BOOST_AUTO_TEST_CASE(b_pointer_from_a_pointer)
{
  apvec_t a;
  std::unique_ptr<B> bp(new B);
  std::unique_ptr<A> ap(new A);
  a.push_back(bp.get());
  a.push_back(ap.get());
  BOOST_CHECK_NO_THROW(art::ensurePointer<B*>(a.begin()));
  BOOST_CHECK_THROW(art::ensurePointer<B*>(a.begin() + 1), art::Exception);
}

// const from const.
BOOST_AUTO_TEST_CASE(const_a_pointer_from_const_a)
{
  avec_t const a(1);
  BOOST_CHECK_NO_THROW(art::ensurePointer<A const*>(a.begin()));
}

BOOST_AUTO_TEST_CASE(const_a_pointer_from_const_a_pointer)
{
  acpvec_t a(1);
  BOOST_CHECK_NO_THROW(art::ensurePointer<A const*>(a.begin()));
}

BOOST_AUTO_TEST_CASE(const_a_pointer_from_const_b)
{
  bvec_t const b(1);
  BOOST_CHECK_NO_THROW(art::ensurePointer<A const*>(b.begin()));
}

BOOST_AUTO_TEST_CASE(const_a_pointer_from_const_b_pointer)
{
  bcpvec_t b(1);
  BOOST_CHECK_NO_THROW(art::ensurePointer<A const*>(b.begin()));
}

BOOST_AUTO_TEST_CASE(const_b_pointer_from_const_a_pointer)
{
  acpvec_t a;
  std::unique_ptr<B> bp(new B);
  std::unique_ptr<A> ap(new A);
  a.push_back(bp.get());
  a.push_back(ap.get());
  BOOST_CHECK_NO_THROW(art::ensurePointer<B const*>(a.begin()));
  BOOST_CHECK_THROW(art::ensurePointer<B const*>(a.begin() + 1),
                    art::Exception);
}

// const from non-const.
BOOST_AUTO_TEST_CASE(const_a_pointer_from_a)
{
  avec_t a(1);
  BOOST_CHECK_NO_THROW(art::ensurePointer<A const*>(a.begin()));
}

BOOST_AUTO_TEST_CASE(const_a_pointer_from_a_pointer)
{
  apvec_t a(1);
  BOOST_CHECK_NO_THROW(art::ensurePointer<A const*>(a.begin()));
}

BOOST_AUTO_TEST_CASE(const_a_pointer_from_b)
{
  bvec_t b(1);
  BOOST_CHECK_NO_THROW(art::ensurePointer<A const*>(b.begin()));
}

BOOST_AUTO_TEST_CASE(const_a_pointer_from_b_pointer)
{
  bpvec_t b(1);
  BOOST_CHECK_NO_THROW(art::ensurePointer<A const*>(b.begin()));
}

BOOST_AUTO_TEST_CASE(const_b_pointer_from_a_pointer)
{
  apvec_t a;
  std::unique_ptr<B> bp(new B);
  std::unique_ptr<A> ap(new A);
  a.push_back(bp.get());
  a.push_back(ap.get());
  BOOST_CHECK_NO_THROW(art::ensurePointer<B const*>(a.begin()));
  BOOST_CHECK_THROW(art::ensurePointer<B const*>(a.begin() + 1),
                    art::Exception);
}

// Assume volatility works the same way.

BOOST_AUTO_TEST_SUITE_END()
