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
#include "TDataMember.h"
#include "TDataType.h"
#include "TDictAttributeMap.h"
#include "TEnum.h"
#include "THashTable.h"
#include "TInterpreter.h"
#include "TList.h"
#include "TROOT.h"
#include "TVirtualCollectionProxy.h"
#include <algorithm>
#include <iostream>
#include <memory>
#include <regex>
#include <set>
#include <sstream>

using namespace cet;
using namespace std;

namespace {

set<string>&
missingTypes()
{
  static boost::thread_specific_ptr<set<string>> missingTypes_;
  if (missingTypes_.get() == nullptr) {
    missingTypes_.reset(new set<string>);
  }
  return *missingTypes_.get();
}

string
template_arg_at(string const& name, unsigned long desired_arg)
{
  string retval;
  auto comma_count = 0ul;
  auto template_level = 0ul;
  auto arg_start = string::npos;
  auto pos = 0ul;
  pos = name.find_first_of("<,>", pos);
  while (pos != string::npos) {
    if (name[pos] == '<') {
      ++template_level;
      if ((desired_arg == 0ul) && (template_level == 1ul)) {
        // Found the begin of the desired template arg.
        arg_start = pos + 1;
      }
    }
    else if (name[pos] == '>') {
      --template_level;
      if ((desired_arg == comma_count) && (template_level == 0ul)) {
        // Found the end of the desired template arg.
        retval = name.substr(arg_start, pos - arg_start);
        return retval;
      }
    }
    else {
      // We have a comma.
      if (template_level == 1ul) {
        // Ignore arguments not at the first level.
        if (comma_count == desired_arg) {
          // Found the end of the desired template arg.
          retval = name.substr(arg_start, pos - arg_start);
          return retval;
        }
        ++comma_count;
        if (comma_count == desired_arg) {
          // Found the begin of the desired template arg.
          arg_start = pos + 1;
        }
      }
    }
    ++pos;
    pos = name.find_first_of("<,>", pos);
  }
  return retval;
}

} // unnamed namespace

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
      string mapped_type_name = template_arg_at(vcname, 1);
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
checkDictionaries(string const& name_orig, bool recursive/*=false*/,
                  int level/*=0*/)
{
  static set<string> checked_names;
  //string indent(level * 2, ' ');
  string name(name_orig);
  //cout << indent << "Checking dictionary for: " << name << endl;
  // Strip leading const.
  if (name.size() > 6) {
    if (!name.compare(0, 6, "const ")) {
      name.erase(0, 6);
    }
  }
  // Strip trailing const.
  if (name.size() > 6) {
    if (!name.compare(name.size() - 6, 6, "const ")) {
      name.erase(name.size() - 6, 6);
    }
  }
  //
  // FIXME: What about volatile & restrict?
  //
  // Strip trailing &&.
  if (name.size() > 2) {
    if (!name.compare(name.size() - 2, 2, "&&")) {
      name.erase(name.size() - 2, 2);
    }
  }
  // Strip trailing &.
  if (name.size() > 1) {
    if (name[name.size()-1] == '&') {
      name.erase(name.size() - 1, 1);
    }
  }
  if (name.empty()) {
    return;
  }
  // Strip trailing *.
  {
    auto pos = name.size();
    while ((pos > 0) && (name[pos-1] == '*')) {
        --pos;
    }
    if (pos == 0) {
      // Name was all '*'.
      return;
    }
    name.erase(pos);
  }
  // Strip leading std::
  if (!name.compare(0, 5, "std::")) {
    if (name.size() == 5) {
      // Name is nothing but "std::".
      return;
    }
    name.erase(0, 5);
  }
  {
    auto I = checked_names.find(name);
    if (I != checked_names.end()) {
      // Already checked this name.
      //cout << indent << "type already checked" << endl;
      return;
    }
    checked_names.insert(name);
  }
  if (!name.compare("void")) {
    //cout << indent << "type is void" << endl;
    return;
  }
  if (name.size() > 12) {
    if (!name.compare(name.size() - 13, 13, "::(anonymous)")) {
      //cout << indent << "type is actually an anonymous namespace name" << endl;
      return;
    }
  }
  TypeWithDict ty(name);
  if (ty) {
    if (ty.category() == TypeWithDict::Category::NONE) {
      //cout << "category: " << "NONE" << endl;
      throw Exception(errors::LogicError, "checkDictionaries: ")
          << "Type category of name is NONE: "
          << name
          << 'n';
    }
    if (ty.category() == TypeWithDict::Category::CLASSTYPE) {
      //cout << "category: " << "CLASSTYPE" << endl;
    }
    if (ty.category() == TypeWithDict::Category::ENUMTYPE) {
      //cout << "category: " << "ENUMTYPE" << endl;
      //cout << indent << "type is an enumeration" << endl;
      return;
    }
    if (ty.category() == TypeWithDict::Category::BASICTYPE) {
      //cout << "category: " << "BASICTYPE" << endl;
      //cout << indent << "type is basic" << endl;
      return;
    }
  }
  auto cl = TClass::GetClass(name.c_str());
  if (cl == nullptr) {
    missingTypes().insert(name);
    // Note: The rest of the code assumes cl is not nullptr.
    return;
  }
  if (!TClass::HasDictionarySelection(name.c_str())) {
    missingTypes().insert(name);
  }
  {
    auto am = cl->GetAttributeMap();
    if (am && am->HasKey("persistent") &&
        am->GetPropertyAsString("persistent") == string("false")) {
      // Marked transient in the selection xml.
      //cout << indent << "class marked not persistent in selection xml" << endl;
      return;
    }
    if (am && am->HasKey("transient") &&
        am->GetPropertyAsString("transient") == string("true")) {
      // Marked transient in the selection xml.
      //cout << indent << "class marked transient in selection xml" << endl;
      return;
    }
  }
#if 0
  {
    THashTable missing;
    cl->GetMissingDictionaries(missing, recursive);
    TClass::GetClass(name.c_str())->GetMissingDictionaries(missing, recursive);
    if (missing.GetEntries()) {
      transform(missing.begin(), missing.end(), inserter(missingTypes(),
      missingTypes().begin()), [](TObject * obj) {
        return dynamic_cast<TClass*>(obj)->GetName();
      });
    }
  }
#endif // 0
  {
    static regex const reNoSplit("^(art::PtrVector(<|Base$)|art::Assns<)");
    if (regex_search(name, reNoSplit)) {
      FDEBUG(1)
          << "Setting NoSplit on class "
          << name
          << "\n";
      //cout << indent << "Setting NoSplit on class " << cl->GetName() << endl;
      cl->SetCanSplit(0);
    }
  }
  if (!recursive) {
    return;
  }
  if (!name.compare(0, 6, "array<")) {
    auto arg0 = template_arg_at(name, 0);
    if (arg0.empty()) {
      throw Exception(errors::LogicError, "checkDictionaries: ")
          << "Could not get first template arg from: "
          << name
          << '\n';
    }
    name = arg0;
    checkDictionaries(arg0, true, level + 2);
    return;
  }
  if (!name.compare(0, 6, "deque<")) {
    auto arg0 = template_arg_at(name, 0);
    if (arg0.empty()) {
      throw Exception(errors::LogicError, "checkDictionaries: ")
          << "Could not get first template arg from: "
          << name
          << '\n';
    }
    checkDictionaries(arg0, true, level + 2);
    return;
  }
  if (!name.compare(0, 13, "forward_list<")) {
    auto arg0 = template_arg_at(name, 0);
    if (arg0.empty()) {
      throw Exception(errors::LogicError, "checkDictionaries: ")
          << "Could not get first template arg from: "
          << name
          << '\n';
    }
    checkDictionaries(arg0, true, level + 2);
    return;
  }
  if (!name.compare(0, 5, "list<")) {
    auto arg0 = template_arg_at(name, 0);
    if (arg0.empty()) {
      throw Exception(errors::LogicError, "checkDictionaries: ")
          << "Could not get first template arg from: "
          << name
          << '\n';
    }
    checkDictionaries(arg0, true, level + 2);
    return;
  }
  if (!name.compare(0, 6, "string")) {
    // Ignore, root has special handling for this.
    return;
  }
  if (!name.compare(0, 9, "u16string")) {
    // Ignore, root has special handling for this.
    // FIXME: It does not!
    return;
  }
  if (!name.compare(0, 9, "u32string")) {
    // Ignore, root has special handling for this.
    // FIXME: It does not!
    return;
  }
  if (!name.compare(0, 7, "wstring")) {
    // Ignore, root has special handling for this.
    // FIXME: It does not!
    return;
  }
  if (!name.compare(0, 13, "basic_string<")) {
    auto arg0 = template_arg_at(name, 0);
    if (arg0.empty()) {
      throw Exception(errors::LogicError, "checkDictionaries: ")
          << "Could not get first template arg from: "
          << name
          << '\n';
    }
    checkDictionaries(arg0, true, level + 2);
    return;
  }
  if (!name.compare(0, 7, "vector<")) {
    auto arg0 = template_arg_at(name, 0);
    if (arg0.empty()) {
      throw Exception(errors::LogicError, "checkDictionaries: ")
          << "Could not get first template arg from: "
          << name
          << '\n';
    }
    checkDictionaries(arg0, true, level + 2);
    return;
  }
  if (!name.compare(0, 4, "map<")) {
    auto arg0 = template_arg_at(name, 0);
    if (arg0.empty()) {
      throw Exception(errors::LogicError, "checkDictionaries: ")
          << "Could not get first template arg from: "
          << name
          << '\n';
    }
    checkDictionaries(arg0, true, level + 2);
    auto arg1 = template_arg_at(name, 1);
    if (arg1.empty()) {
      throw Exception(errors::LogicError, "checkDictionaries: ")
          << "Could not get second template arg from: "
          << name
          << '\n';
    }
    checkDictionaries(arg1, true, level + 2);
    //FIXME: Should check Compare, and Allocator too!
    return;
  }
  if (!name.compare(0, 9, "multimap<")) {
    auto arg0 = template_arg_at(name, 0);
    if (arg0.empty()) {
      throw Exception(errors::LogicError, "checkDictionaries: ")
          << "Could not get first template arg from: "
          << name
          << '\n';
    }
    checkDictionaries(arg0, true, level + 2);
    auto arg1 = template_arg_at(name, 1);
    if (arg1.empty()) {
      throw Exception(errors::LogicError, "checkDictionaries: ")
          << "Could not get second template arg from: "
          << name
          << '\n';
    }
    checkDictionaries(arg1, true, level + 2);
    //FIXME: Should check Compare, and Allocator too!
    return;
  }
  if (!name.compare(0, 14, "unordered_map<")) {
    auto arg0 = template_arg_at(name, 0);
    if (arg0.empty()) {
      throw Exception(errors::LogicError, "checkDictionaries: ")
          << "Could not get first template arg from: "
          << name
          << '\n';
    }
    checkDictionaries(arg0, true, level + 2);
    auto arg1 = template_arg_at(name, 1);
    if (arg1.empty()) {
      throw Exception(errors::LogicError, "checkDictionaries: ")
          << "Could not get second template arg from: "
          << name
          << '\n';
    }
    checkDictionaries(arg1, true, level + 2);
    //FIXME: Should check Hash, Pred, and Allocator too!
    return;
  }
  if (!name.compare(0, 19, "unordered_multimap<")) {
    auto arg0 = template_arg_at(name, 0);
    if (arg0.empty()) {
      throw Exception(errors::LogicError, "checkDictionaries: ")
          << "Could not get first template arg from: "
          << name
          << '\n';
    }
    checkDictionaries(arg0, true, level + 2);
    auto arg1 = template_arg_at(name, 1);
    if (arg1.empty()) {
      throw Exception(errors::LogicError, "checkDictionaries: ")
          << "Could not get second template arg from: "
          << name
          << '\n';
    }
    checkDictionaries(arg1, true, level + 2);
    //FIXME: Should check Hash, Pred, and Allocator too!
    return;
  }
  if (!name.compare(0, 4, "set<")) {
    auto arg0 = template_arg_at(name, 0);
    if (arg0.empty()) {
      throw Exception(errors::LogicError, "checkDictionaries: ")
          << "Could not get first template arg from: "
          << name
          << '\n';
    }
    checkDictionaries(arg0, true, level + 2);
    //FIXME: Should check Compare, and Allocator too!
    return;
  }
  if (!name.compare(0, 9, "multiset<")) {
    auto arg0 = template_arg_at(name, 0);
    if (arg0.empty()) {
      throw Exception(errors::LogicError, "checkDictionaries: ")
          << "Could not get first template arg from: "
          << name
          << '\n';
    }
    checkDictionaries(arg0, true, level + 2);
    //FIXME: Should check Compare, and Allocator too!
    return;
  }
  if (!name.compare(0, 14, "unordered_set<")) {
    auto arg0 = template_arg_at(name, 0);
    if (arg0.empty()) {
      throw Exception(errors::LogicError, "checkDictionaries: ")
          << "Could not get first template arg from: "
          << name
          << '\n';
    }
    checkDictionaries(arg0, true, level + 2);
    //FIXME: Should check Hash, Pred, and Allocator too!
    return;
  }
  if (!name.compare(0, 19, "unordered_multiset<")) {
    auto arg0 = template_arg_at(name, 0);
    if (arg0.empty()) {
      throw Exception(errors::LogicError, "checkDictionaries: ")
          << "Could not get first template arg from: "
          << name
          << '\n';
    }
    checkDictionaries(arg0, true, level + 2);
    //FIXME: Should check Hash, Pred, and Allocator too!
    return;
  }
  if (!name.compare(0, 6, "queue<")) {
    auto arg0 = template_arg_at(name, 0);
    if (arg0.empty()) {
      throw Exception(errors::LogicError, "checkDictionaries: ")
          << "Could not get first template arg from: "
          << name
          << '\n';
    }
    checkDictionaries(arg0, true, level + 2);
    //FIXME: Should check Container too!
    return;
  }
  if (!name.compare(0, 15, "priority_queue<")) {
    auto arg0 = template_arg_at(name, 0);
    if (arg0.empty()) {
      throw Exception(errors::LogicError, "checkDictionaries: ")
          << "Could not get first template arg from: "
          << name
          << '\n';
    }
    checkDictionaries(arg0, true, level + 2);
    //FIXME: Should check Container, and Compare too!
    return;
  }
  if (!name.compare(0, 6, "stack<")) {
    auto arg0 = template_arg_at(name, 0);
    if (arg0.empty()) {
      throw Exception(errors::LogicError, "checkDictionaries: ")
          << "Could not get first template arg from: "
          << name
          << '\n';
    }
    checkDictionaries(arg0, true, level + 2);
    //FIXME: Should check Container too!
    return;
  }
  for (auto obj : *cl->GetListOfBases()) {
    auto bc = dynamic_cast<TBaseClass*>(obj);
    //cout << indent << "  Base class: " << bc->GetName() << endl;
    checkDictionaries(bc->GetName(), true, level + 2);
  }
  for (auto obj : *cl->GetListOfDataMembers()) {
    auto dm = dynamic_cast<TDataMember*>(obj);
    //cout << indent << "  Data member: " << dm->GetName() << endl;
    if (!dm->IsPersistent()) {
      // The data member comment in the header file starts with '!'.
      //cout << indent << "  marked not persistent in header file" << endl;
      continue;
    }
    if (dm->Property() & kIsStatic) {
      // Static data member.
      //cout << indent << "  static" << endl;
      continue;
    }
    auto am = dm->GetAttributeMap();
    if (am && am->HasKey("persistent") &&
        am->GetPropertyAsString("persistent") == string("false")) {
      // Marked transient in the selection xml.
      //cout << indent << "  marked not persistent in selection xml" << endl;
      continue;
    }
    if (am && am->HasKey("transient") &&
        am->GetPropertyAsString("transient") == string("true")) {
      // Marked transient in the selection xml.
      //cout << indent << "  marked transient in selection xml" << endl;
      continue;
    }
    if (am && am->HasKey("comment") &&
        (am->GetPropertyAsString("comment")[0] == '!')) {
      // Marked transient in the selection xml.
      //cout << indent << "  commented transient in selection xml" << endl;
      continue;
    }
    checkDictionaries(dm->GetTrueTypeName(), true, level + 2);
  }
}

void
reportFailedDictionaryChecks()
{
  if (missingTypes().empty()) {
    return;
  }
  ostringstream ostr;
  for (auto I = missingTypes().cbegin(), E = missingTypes().cend();
       I != E; ++I) {
    ostr << "     " << cet::demangle_symbol(*I) << "\n";
  }
  throw Exception(errors::DictionaryNotFound)
      << "No dictionary found for the following classes:\n\n"
      << ostr.str()
      << "\nMost likely they were never generated, but it may be that they "
         "were generated in the wrong package.\n"
         "\n"
         "Please add (or move) the specification\n"
         "\n"
         "     <class name=\"MyClassName\"/>\n"
         "\n"
         "to the appropriate classes_def.xml file.\n"
         "\n"
         "If the class is a template instance, you may need\n"
         "to define a dummy variable of this type in classes.h.\n"
         "\n"
         "Also, if this class has any transient members,\n"
         "you need to specify them in classes_def.xml.";
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
          pos = template_instance.find_last_not_of(" \t", pos - 1) + 1;
          result = template_instance.substr(arg_start, pos - arg_start);
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

