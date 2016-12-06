#include "canvas/Persistency/Provenance/TypeTools.h"
// vim: set sw=2:

#include "canvas/Persistency/Provenance/TypeWithDict.h"
#include "canvas/Utilities/DebugMacros.h"
#include "canvas/Utilities/Exception.h"
#include "boost/algorithm/string.hpp"
#include "boost/thread/tss.hpp"
#include "cetlib/container_algorithms.h"
#include "cetlib/demangle.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "ESTLType.h"
#include "TBaseClass.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TList.h"
#include "TVirtualCollectionProxy.h"

#include <algorithm>
#include <iostream>
#include <memory>

using namespace cet;
using namespace std;

namespace art {

TypeWithDict
find_nested_type_named(string const& nested_type, TClass* const type_to_search)
{
  if (type_to_search == nullptr) {
    throw Exception(errors::NullPointerError, "find_nested_type_named: ")
        << "Null TClass pointer passed for type_to_search!\n";
  }
  TypeWithDict
    found_type(string(type_to_search->GetName()) + "::" +
               nested_type);
  return found_type;
}

TypeWithDict
value_type_of(TClass* const t)
{
  TypeWithDict found_type;
  if (t == nullptr) {
    throw Exception(errors::NullPointerError, "value_type_of: ")
        << "Null TClass pointer passed for type!\n";
  }
  ROOT::ESTLType stlty = t->GetCollectionType();
  if ((stlty > ROOT::kNotSTL) && (stlty < ROOT::kSTLend)) {
    TVirtualCollectionProxy* vcp = t->GetCollectionProxy();
    TClass* vc = vcp->GetValueClass();
    if (vc != nullptr) {
      found_type = TypeWithDict(*vc->GetTypeInfo());
      return found_type;
    }
    auto vty = static_cast<int>(vcp->GetType());
    found_type = TypeWithDict(getTypeID(vty));
    return found_type;
  }
  // Look for "value_type" attribute in TClass.
  auto am = t->GetAttributeMap();
  if (am && am->HasKey("value_type")) {
    string vt_name(am->GetPropertyAsString("value_type"));
    found_type = TypeWithDict(vt_name);
  }
  return found_type;
}

TypeWithDict
mapped_type_of(TClass* const t)
{
  TypeWithDict found_type;
  if (t == nullptr) {
    throw Exception(errors::NullPointerError, "mapped_type_of: ")
        << "Null TClass pointer passed for type!\n";
  }
  ROOT::ESTLType stlty = t->GetCollectionType();
  if ((stlty == ROOT::kSTLmap) ||
      (stlty == ROOT::kSTLmultimap) ||
      (stlty == ROOT::kSTLunorderedmap) ||
      (stlty == ROOT::kSTLunorderedmultimap)) {
    TVirtualCollectionProxy* vcp = t->GetCollectionProxy();
    TClass* vc = vcp->GetValueClass();
    if (vc != nullptr) {
      string vcname(vc->GetName());
      string mapped_type_name = name_of_template_arg(vcname, 1);
      found_type = TypeWithDict(mapped_type_name);
      return found_type;
    }
    // This should be impossible.
    throw Exception(errors::LogicError)
        << "ROOT map type did not have a value class!\n";
  }
  // Look for "mapped_type" attribute in TClass.
  auto am = t->GetAttributeMap();
  if (am && am->HasKey("mapped_type")) {
    string mt_name(am->GetPropertyAsString("mapped_type"));
    found_type = TypeWithDict(mt_name);
  }
  return found_type;
}

void
public_base_classes(TClass* const cl, vector<TClass*>& baseTypes)
{
  if (cl == nullptr) {
    throw Exception(errors::NullPointerError, "public_base_classes: ")
        << "Null TClass pointer passed!\n";
  }
  for (auto bobj : *cl->GetListOfBases()) {
    auto bb = dynamic_cast<TBaseClass*>(bobj);
    if (bb->Property() & kIsPublic) {
      baseTypes.push_back(bb->GetClassPointer());
    }
  }
}

std::string
name_of_template_arg(std::string const & template_instance,
                     size_t desired_arg)
{
  std::string result;
  auto comma_count = 0ul;
  auto template_level = 0ul;
  auto arg_start = string::npos;
  auto pos = 0ul;
  pos = template_instance.find_first_of("<>,", pos);
  while (pos != string::npos) {
    switch (template_instance[pos]) {
      case '<':
        ++template_level;
        if ((desired_arg == 0ul) && (template_level == 1ul)) {
          // Found the begin of the desired template arg.
          arg_start = pos + 1;
        }
        break;
      case '>':
        --template_level;
        if ((desired_arg == comma_count) && (template_level == 0ul)) {
          // Found the end of the desired template arg -- trim trailing whitespace
          auto const arg_end = template_instance.find_last_not_of(" \t", pos - 1) + 1;
          result =
            template_instance.substr(arg_start,
                                     arg_end - arg_start);
          return result;
        }
        break;
      case ',':
        if (template_level != 1ul) {
          // Ignore arguments not at the first level.
          break;
        }
        if (comma_count == desired_arg) {
          // Found the end of the desired template arg.
          result = template_instance.substr(arg_start, pos - arg_start);
          return result;
        }
        ++comma_count;
        if (comma_count == desired_arg) {
          // Found the begin of the desired template arg.
          arg_start = pos + 1;
        }
        break;
    }
    ++pos;
    pos = template_instance.find_first_of("<>,", pos);
  }
  return result;
}

TypeWithDict
type_of_template_arg(TClass* template_instance, size_t desired_arg)
{
  if (template_instance == nullptr) {
    throw Exception(errors::NullPointerError, "type_of_template_arg: ")
        << "Null TClass pointer passed!\n";
  }
  TypeWithDict found_type =
    type_of_template_arg(template_instance->GetName(), desired_arg);
  return found_type;
}

std::string
name_of_assns_partner(std::string assns_type_name) {
//**/cout <<  "-----> Begin art::name_of_assns_partner(assns_type_name)" << endl;
//**/cout <<  "assns_type_name = " << assns_type_name << endl;
  using namespace std::string_literals;
  std::string result;
  static std::string const assns_start = "art::Assns<"s;
//**/cout <<  "Comparing " << assns_start << " with " << assns_type_name.substr(0, assns_start.size()) << endl;
  if (assns_type_name.compare(0, assns_start.size(), assns_start) != 0) {
    // Not an Assns.
//**/cout <<  "-----> End   art::name_of_assns_partner(assns_type_name)" << endl;
    return result;
  }
  auto const arg0 = name_of_template_arg(assns_type_name, 0);
//**/cout <<  "arg0 = " << arg0 << endl;
  auto const arg1 = name_of_template_arg(assns_type_name, 1);
//**/cout <<  "arg1 = " << arg1 << endl;
  auto const arg2 = name_of_template_arg(assns_type_name, 2);
//**/cout <<  "arg2 = " << arg2 << endl;
  result = assns_start + arg1 + ',' + arg0 + ',' + arg2 + '>';
//**/cout << "result = " << result << endl;
//**/cout <<  "-----> End   art::name_of_assns_partner(assns_type_name)" << endl;
  return result;
}

std::string
name_of_assns_base(std::string assns_type_name) {
  using namespace std::string_literals;
  std::string result;
  static std::string const assns_start = "art::Assns<"s;
  if (assns_type_name.compare(0, assns_start.size(), assns_start) != 0) {
    // Not an Assns.
    return result;
  }
  if (name_of_template_arg(assns_type_name, 2) == "void"s) {
    // Doesn't have the base we're looking for.
    return result;
  }
  result = assns_start +
           name_of_template_arg(assns_type_name, 0) +
           ',' +
           name_of_template_arg(assns_type_name, 1) +
           ",void>";
  return result;
}

bool
is_instantiation_of(TClass* const cl, string const& template_name)
{
  if (cl == nullptr) {
    throw Exception(errors::NullPointerError, "is_instantiation_of: ")
        << "Null TClass pointer passed!\n";
  }
  return string(cl->GetName()).find(template_name + "<") == 0ul;
}

void
throwLateDictionaryError(std::string const & className)
{
  throw Exception(errors::LogicError,
                  "NoDictionary: ")
    << "Could not find dictionary for: "
    << className
    << "\ndespite passing runtime dictionary checks.\n";
}

} // namespace art

