#include "canvas/Persistency/Provenance/DictionaryChecker.h"
#include "canvas/Persistency/Provenance/TypeTools.h"
#include "canvas/Persistency/Provenance/TypeWithDict.h"
#include "canvas/Utilities/DebugMacros.h"
#include "canvas/Utilities/Exception.h"
#include "cetlib_except/demangle.h"

#include "TBaseClass.h"
#include "TClass.h"
#include "TClassEdit.h"
#include "TDataMember.h"
#include "TDataType.h"
#include "TDictAttributeMap.h"
#include "TEnum.h"
#include "TList.h"

#include <algorithm>
#include <regex>
#include <sstream>

void
art::DictionaryChecker::checkDictionaries(std::string const& name_orig,
                                          bool const recursive,
                                          int const level)
{
  using namespace std;
  //string const indent(level * 2, ' ');
  string name;
  TClassEdit::GetNormalizedName(name, name_orig);
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
  if (!name.compare(0, 11, "unique_ptr<")) {
    auto const arg0 = name_of_template_arg(name, 0);
    if (arg0.empty()) {
      throw Exception(errors::LogicError, "checkDictionaries: ")
        << "Could not get first template arg from: "
        << name
        << '\n';
    }
    checkDictionaries(arg0, true, level + 2);
    return;
  }
  {
    auto I = checked_names_.find(name);
    if (I != checked_names_.end()) {
      // Already checked this name.
      return;
    }
    checked_names_.insert(name);
  }
  if (!name.compare("void")) {
    return;
  }
  if (name.size() > 12) {
    if (!name.compare(name.size() - 13, 13, "::(anonymous)")) {
      return;
    }
  }
  TypeWithDict ty(name);
  if (ty) {
    if (ty.category() == TypeWithDict::Category::NONE) {
      throw Exception(errors::LogicError, "checkDictionaries: ")
        << "Type category of name is NONE: "
        << name
        << 'n';
    }
    if (ty.category() == TypeWithDict::Category::CLASSTYPE) {
    }
    if (ty.category() == TypeWithDict::Category::ENUMTYPE) {
      return;
    }
    if (ty.category() == TypeWithDict::Category::BASICTYPE) {
      return;
    }
  }
  auto cl = TClass::GetClass(name.c_str());
  if (cl == nullptr) {
    missing_types_.insert(name);
    // Note: The rest of the code assumes cl is not nullptr.
    return;
  }
  if (!TClass::HasDictionarySelection(name.c_str())) {
    missing_types_.insert(name);
  }
  {
    auto am = cl->GetAttributeMap();
    if (am && am->HasKey("persistent") &&
        am->GetPropertyAsString("persistent") == string("false")) {
      // Marked transient in the selection xml.
      return;
    }
    if (am && am->HasKey("transient") &&
        am->GetPropertyAsString("transient") == string("true")) {
      // Marked transient in the selection xml.
      return;
    }
  }
  {
    static regex const reNoSplit("^(art::PtrVector(<|Base$)|art::Assns<)");
    if (regex_search(name, reNoSplit)) {
      FDEBUG(1)
        << "Setting NoSplit on class "
        << name
        << "\n";
      cl->SetCanSplit(0);
    }
  }
  if (!recursive) {
    return;
  }
  if (!name.compare(0, 6, "array<")) {
    auto const arg0 = name_of_template_arg(name, 0);
    if (arg0.empty()) {
      throw Exception(errors::LogicError, "checkDictionaries: ")
        << "Could not get first template arg from: "
        << name
        << '\n';
    }
    checkDictionaries(arg0, true, level + 2);
    return;
  }
  if (!name.compare(0, 6, "deque<")) {
    auto const arg0 = name_of_template_arg(name, 0);
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
    auto const arg0 = name_of_template_arg(name, 0);
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
    auto const arg0 = name_of_template_arg(name, 0);
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
    auto const arg0 = name_of_template_arg(name, 0);
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
    auto const arg0 = name_of_template_arg(name, 0);
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
    auto const arg0 = name_of_template_arg(name, 0);
    if (arg0.empty()) {
      throw Exception(errors::LogicError, "checkDictionaries: ")
        << "Could not get first template arg from: "
        << name
        << '\n';
    }
    checkDictionaries(arg0, true, level + 2);
    auto const arg1 = name_of_template_arg(name, 1);
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
    auto const arg0 = name_of_template_arg(name, 0);
    if (arg0.empty()) {
      throw Exception(errors::LogicError, "checkDictionaries: ")
        << "Could not get first template arg from: "
        << name
        << '\n';
    }
    checkDictionaries(arg0, true, level + 2);
    auto const arg1 = name_of_template_arg(name, 1);
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
    auto const arg0 = name_of_template_arg(name, 0);
    if (arg0.empty()) {
      throw Exception(errors::LogicError, "checkDictionaries: ")
        << "Could not get first template arg from: "
        << name
        << '\n';
    }
    checkDictionaries(arg0, true, level + 2);
    auto const arg1 = name_of_template_arg(name, 1);
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
    auto const arg0 = name_of_template_arg(name, 0);
    if (arg0.empty()) {
      throw Exception(errors::LogicError, "checkDictionaries: ")
        << "Could not get first template arg from: "
        << name
        << '\n';
    }
    checkDictionaries(arg0, true, level + 2);
    auto const arg1 = name_of_template_arg(name, 1);
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
    auto const arg0 = name_of_template_arg(name, 0);
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
    auto const arg0 = name_of_template_arg(name, 0);
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
    auto const arg0 = name_of_template_arg(name, 0);
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
    auto const arg0 = name_of_template_arg(name, 0);
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
    auto const arg0 = name_of_template_arg(name, 0);
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
    auto const arg0 = name_of_template_arg(name, 0);
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
    auto const arg0 = name_of_template_arg(name, 0);
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
    checkDictionaries(bc->GetName(), true, level + 2);
  }
  for (auto obj : *cl->GetListOfDataMembers()) {
    auto dm = dynamic_cast<TDataMember*>(obj);
    if (!dm->IsPersistent()) {
      // The data member comment in the header file starts with '!'.
      continue;
    }
    if (dm->Property() & kIsStatic) {
      // Static data member.
      continue;
    }
    auto am = dm->GetAttributeMap();
    if (am && am->HasKey("persistent") &&
        am->GetPropertyAsString("persistent") == string("false")) {
      // Marked transient in the selection xml.
      continue;
    }
    if (am && am->HasKey("transient") &&
        am->GetPropertyAsString("transient") == string("true")) {
      // Marked transient in the selection xml.
      continue;
    }
    if (am && am->HasKey("comment") &&
        (am->GetPropertyAsString("comment")[0] == '!')) {
      // Marked transient in the selection xml.
      continue;
    }
    checkDictionaries(dm->GetTrueTypeName(), true, level + 2);
  }
}

std::vector<std::string>
art::DictionaryChecker::typesMissingDictionaries()
{
  std::vector<std::string> result;
  for (auto const& mt : missing_types_) {
    result.emplace_back(cet::demangle_symbol(mt));
  }
  resetMissingTypes_();
  std::sort(result.begin(), result.end());
  return result;
}

void
art::DictionaryChecker::reportMissingDictionaries()
{
  using namespace std;
  if (missing_types_.empty()) {
    return;
  }
  ostringstream ostr;
  for (auto const& mt : typesMissingDictionaries()) {
    ostr << "     " << mt << "\n";
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
    "Also, if this class has any transient members,\n"
    "you need to specify them in classes_def.xml.";
}
