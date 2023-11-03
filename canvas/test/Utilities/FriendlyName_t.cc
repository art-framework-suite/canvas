#define BOOST_TEST_MODULE (FriendlyName_t)
#include "boost/test/unit_test.hpp"

#include "canvas/Utilities/FriendlyName.h"

#include <map>
#include <string>

namespace {
  using fnmap_t = std::map<std::string, std::string>;
}

struct FriendlyNameTestFixture {
  FriendlyNameTestFixture();

  fnmap_t nameMap;
};

FriendlyNameTestFixture::FriendlyNameTestFixture() : nameMap()
{
  nameMap.try_emplace("Foo", "Foo");
  nameMap.try_emplace("bar::Foo", "bar::Foo");
  nameMap.try_emplace("std::vector<Foo>", "Foos");
  nameMap.try_emplace("std::vector<bar::Foo>", "bar::Foos");
  nameMap.try_emplace("V<A,B>", "ABV");
  nameMap.try_emplace("art::Wrapper<MuonDigiCollection<CSCDetId,CSCALCTDigi> >",
                      "CSCDetIdCSCALCTDigiMuonDigiCollection");
  nameMap.try_emplace("A<B<C>,D<E> >", "CBEDA");
  nameMap.try_emplace("A<B<C<D> > >", "DCBA");
  nameMap.try_emplace("A<B<C,D>,E<F> >", "CDBFEA");
  nameMap.try_emplace("Aa<Bb<Cc>,Dd<Ee> >", "CcBbEeDdAa");
  nameMap.try_emplace("Aa<Bb<Cc<Dd> > >", "DdCcBbAa");
  nameMap.try_emplace("Aa<Bb<Cc,Dd>,Ee<Ff> >", "CcDdBbFfEeAa");
  nameMap.try_emplace("Aa<Bb<Cc,Dd>,Ee<Ff,Gg> >", "CcDdBbFfGgEeAa");
  nameMap.try_emplace("cet::map_vector_key", "mvk");
  nameMap.try_emplace("cet::map_vector<Foo>", "Foomv");
  nameMap.try_emplace("art::Assns<Ll,Rr,Dd>", "LlRrDdart::Assns");
  nameMap.try_emplace("art::Assns<Rr,Ll,Dd>", "LlRrDdart::Assns");
}

BOOST_FIXTURE_TEST_SUITE(FriendlyName_t, FriendlyNameTestFixture)

BOOST_AUTO_TEST_CASE(FriendlyName_t)
{
  for (auto const& [type_name, friendly_name] : nameMap) {
    BOOST_TEST(art::friendlyname::friendlyName(type_name) == friendly_name);
  }
}

BOOST_AUTO_TEST_SUITE_END()
