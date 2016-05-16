// Test of the TypeTools functions.

#define BOOST_TEST_MODULE (TypeTools_t)
#include "cetlib/quiet_unit_test.hpp"

#include <iostream>
#include <typeinfo>
#include <vector>

#include "canvas/Persistency/Provenance/TypeTools.h"
#include "canvas/Persistency/Provenance/TypeWithDict.h"
#include "canvas/Persistency/Common/Wrapper.h"

BOOST_AUTO_TEST_SUITE(TypeTools_t)

BOOST_AUTO_TEST_CASE(no_dictionary_is_invalid)
{
  BOOST_REQUIRE(!TClass::GetClass("ThereIsNoTypeWithThisName"));
}

BOOST_AUTO_TEST_CASE(find_nested)
{
  TClass* intvec(TClass::GetClass("std::vector<int>"));
  BOOST_REQUIRE(intvec);

  BOOST_REQUIRE(!art::find_nested_type_named("WankelRotaryEngine",
                                             intvec));
  BOOST_REQUIRE(art::find_nested_type_named("const_iterator",
                                            intvec));
}

BOOST_AUTO_TEST_CASE(burrowing)
{
  TClass* wrapper_type(TClass::GetClass(typeid(std::vector<int>)));
  BOOST_REQUIRE(wrapper_type);
  art::TypeWithDict
    v_type(art::find_nested_type_named("value_type", wrapper_type));
  BOOST_REQUIRE(v_type);
  BOOST_REQUIRE_EQUAL(v_type.className(), std::string("int"));
}

BOOST_AUTO_TEST_CASE(type_of_template_arg)
{
  TClass * wrapper(TClass::GetClass("art::Wrapper<std::vector<int>>"));
  BOOST_REQUIRE(wrapper);
  art::TypeWithDict ty = art::type_of_template_arg(wrapper, 0);
  BOOST_REQUIRE(ty);
  TClass* arg(ty.tClass());
  BOOST_REQUIRE(arg);
  BOOST_REQUIRE_EQUAL(arg->GetName(), std::string("vector<int>"));
}

BOOST_AUTO_TEST_CASE(is_instantiation_of)
{
  TClass* wrapper(TClass::GetClass("art::Wrapper<int>"));
  BOOST_REQUIRE(wrapper);
  BOOST_REQUIRE(art::is_instantiation_of(wrapper, "art::Wrapper"));
}

BOOST_AUTO_TEST_SUITE_END()
