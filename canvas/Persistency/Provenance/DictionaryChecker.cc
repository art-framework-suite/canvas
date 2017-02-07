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
art::DictionaryChecker::
checkDictionaries(std::string const & name_orig,
                  bool recursive,
                  int level)
{
  using namespace std;
  //string const indent(level * 2, ' ');
  string name;
  TClassEdit::GetNormalizedName(name, name_orig);
  //cerr << indent << "Checking dictionary for: " << name << " (normalized from " << name_orig << ")\n";
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
    auto I = checked_names_.find(name);
    if (I != checked_names_.end()) {
      // Already checked this name.
      //cerr << indent << "type already checked" << "\n";
      return;
    }
    checked_names_.insert(name);
  }
  if (!name.compare("void")) {
    //cerr << indent << "type is void" << "\n";
    return;
  }
  if (name.size() > 12) {
    if (!name.compare(name.size() - 13, 13, "::(anonymous)")) {
      //cerr << indent << "type is actually an anonymous namespace name" << "\n";
      return;
    }
  }
  TypeWithDict ty(name);
  if (ty) {
    if (ty.category() == TypeWithDict::Category::NONE) {
      //cerr << "category: " << "NONE" << "\n";
      throw Exception(errors::LogicError, "checkDictionaries: ")
          << "Type category of name is NONE: "
          << name
          << 'n';
    }
    if (ty.category() == TypeWithDict::Category::CLASSTYPE) {
      //cerr << indent << "category: " << "CLASSTYPE" << "\n";
    }
    if (ty.category() == TypeWithDict::Category::ENUMTYPE) {
      //cerr << indent << "category: " << "ENUMTYPE" << "\n";
      //cerr << indent << "type is an enumeration" << "\n";
      return;
    }
    if (ty.category() == TypeWithDict::Category::BASICTYPE) {
      //cerr << indent << "category: " << "BASICTYPE" << "\n";
      //cerr << indent << "type is basic" << "\n";
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
      //cerr << indent << "class marked not persistent in selection xml" << "\n";
      return;
    }
    if (am && am->HasKey("transient") &&
        am->GetPropertyAsString("transient") == string("true")) {
      // Marked transient in the selection xml.
      //cerr << indent << "class marked transient in selection xml" << "\n";
      return;
    }
  }
#if 0
  {
    THashTable missing;
    cl->GetMissingDictionaries(missing, recursive);
    TClass::GetClass(name.c_str())->GetMissingDictionaries(missing, recursive);
    if (missing.GetEntries()) {
      transform(missing.begin(), missing.end(), inserter(missing_types_,
      missing_types_.begin()), [](TObject * obj) {
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
      //cerr << indent << "Setting NoSplit on class " << cl->GetName() << "\n";
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
    //cerr << indent << name << " (array) -> " << arg0 << "\n";
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
    //cerr << indent << name << " (deque) -> " << arg0 << "\n";
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
    //cerr << indent << name << " (forward_list) -> " << arg0 << "\n";
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
    //cerr << indent << name << " (list) -> " << arg0 << "\n";
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
    //cerr << indent << name << " (basic_string) -> " << arg0 << "\n";
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
    //cerr << indent << name << " (vector) -> " << arg0 << "\n";
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
    //cerr << indent << name << " (map) -> " << arg0 << " (key)\n";
    checkDictionaries(arg0, true, level + 2);
    auto const arg1 = name_of_template_arg(name, 1);
    if (arg1.empty()) {
      throw Exception(errors::LogicError, "checkDictionaries: ")
          << "Could not get second template arg from: "
          << name
          << '\n';
    }
    //cerr << indent << name << " (map) -> " << arg1 << " (value)\n";
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
    //cerr << indent << name << " (multimap) -> " << arg0 << " (key)\n";
    checkDictionaries(arg0, true, level + 2);
    auto const arg1 = name_of_template_arg(name, 1);
    if (arg1.empty()) {
      throw Exception(errors::LogicError, "checkDictionaries: ")
          << "Could not get second template arg from: "
          << name
          << '\n';
    }
    //cerr << indent << name << " (multimap) -> " << arg1 << " (key)\n";
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
    //cerr << indent << name << " (unordered_map) -> " << arg0 << " (key)\n";
    checkDictionaries(arg0, true, level + 2);
    auto const arg1 = name_of_template_arg(name, 1);
    if (arg1.empty()) {
      throw Exception(errors::LogicError, "checkDictionaries: ")
          << "Could not get second template arg from: "
          << name
          << '\n';
    }
    //cerr << indent << name << " (unordered_map) -> " << arg1 << " (key)\n";
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
    //cerr << indent << name << " (unordered_multimap) -> " << arg0 << " (key)\n";
    checkDictionaries(arg0, true, level + 2);
    auto const arg1 = name_of_template_arg(name, 1);
    if (arg1.empty()) {
      throw Exception(errors::LogicError, "checkDictionaries: ")
          << "Could not get second template arg from: "
          << name
          << '\n';
    }
    //cerr << indent << name << " (unordered_multimap) -> " << arg1 << " (key)\n";
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
    //cerr << indent << name << " (set) -> " << arg0 << "\n";
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
    //cerr << indent << name << " (multiset) -> " << arg0 << "\n";
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
    //cerr << indent << name << " (unordered_set) -> " << arg0 << "\n";
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
    //cerr << indent << name << " (unordered_multiset) -> " << arg0 << "\n";
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
    //cerr << indent << name << " (queue) -> " << arg0 << "\n";
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
    //cerr << indent << name << " (priority_queue) -> " << arg0 << "\n";
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
    //cerr << indent << name << " (stack) -> " << arg0 << "\n";
    checkDictionaries(arg0, true, level + 2);
    //FIXME: Should check Container too!
    return;
  }
  for (auto obj : *cl->GetListOfBases()) {
    auto bc = dynamic_cast<TBaseClass*>(obj);
    //cerr << indent << name << " -> " << bc->GetName() << " (base)\n";
    checkDictionaries(bc->GetName(), true, level + 2);
  }
  for (auto obj : *cl->GetListOfDataMembers()) {
    auto dm = dynamic_cast<TDataMember*>(obj);
    //cerr << indent << name << " -> " << dm->GetName() << " (member)\n";
    if (!dm->IsPersistent()) {
      // The data member comment in the header file starts with '!'.
      //cerr << indent << "  marked not persistent in header file" << "\n";
      continue;
    }
    if (dm->Property() & kIsStatic) {
      // Static data member.
      //cerr << indent << "  static" << "\n";
      continue;
    }
    auto am = dm->GetAttributeMap();
    if (am && am->HasKey("persistent") &&
        am->GetPropertyAsString("persistent") == string("false")) {
      // Marked transient in the selection xml.
      //cerr << indent << "  marked not persistent in selection xml" << "\n";
      continue;
    }
    if (am && am->HasKey("transient") &&
        am->GetPropertyAsString("transient") == string("true")) {
      // Marked transient in the selection xml.
      //cerr << indent << "  marked transient in selection xml" << "\n";
      continue;
    }
    if (am && am->HasKey("comment") &&
        (am->GetPropertyAsString("comment")[0] == '!')) {
      // Marked transient in the selection xml.
      //cerr << indent << "  commented transient in selection xml" << "\n";
      continue;
    }
    //cerr << indent << name << " -> " << dm->GetTrueTypeName() << " (member)\n";
    checkDictionaries(dm->GetTrueTypeName(), true, level + 2);
  }
}

std::vector<std::string>
art::DictionaryChecker::
typesMissingDictionaries()
{
  std::vector<std::string> result;
  for (auto const & mt : missing_types_) {
    result.emplace_back(cet::demangle_symbol(mt));
  }
  resetMissingTypes_();
  std::sort(result.begin(), result.end());
  return result;
}

void
art::DictionaryChecker::
reportMissingDictionaries()
{
  using namespace std;
  if (missing_types_.empty()) {
    return;
  }
  ostringstream ostr;
  for (auto const & mt : typesMissingDictionaries()) {
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
