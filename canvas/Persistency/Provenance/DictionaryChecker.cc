#include "canvas/Persistency/Provenance/DictionaryChecker.h"
#include "canvas/Persistency/Provenance/TypeTools.h"
#include "canvas/Persistency/Provenance/TypeWithDict.h"
#include "canvas/Utilities/DebugMacros.h"
#include "canvas/Utilities/Exception.h"
#include "cetlib/demangle.h"

#include "TBaseClass.h"
#include "TClass.h"
#include "TDataMember.h"
#include "TDataType.h"
#include "TDictAttributeMap.h"
#include "TEnum.h"
#include "TList.h"

#include <regex>
#include <sstream>

void
art::DictionaryChecker::
checkDictionaries(std::string const & name_orig,
                  bool recursive,
                  int level)
{
  using namespace std;
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
    auto I = checked_names_.find(name);
    if (I != checked_names_.end()) {
      // Already checked this name.
      //cout << indent << "type already checked" << endl;
      return;
    }
    checked_names_.insert(name);
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
      //cout << indent << "Setting NoSplit on class " << cl->GetName() << endl;
      cl->SetCanSplit(0);
    }
  }
  if (!recursive) {
    return;
  }
  if (!name.compare(0, 6, "array<")) {
    auto arg0 = name_of_template_arg(name, 0);
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
    auto arg0 = name_of_template_arg(name, 0);
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
    auto arg0 = name_of_template_arg(name, 0);
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
    auto arg0 = name_of_template_arg(name, 0);
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
    auto arg0 = name_of_template_arg(name, 0);
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
    auto arg0 = name_of_template_arg(name, 0);
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
    auto arg0 = name_of_template_arg(name, 0);
    if (arg0.empty()) {
      throw Exception(errors::LogicError, "checkDictionaries: ")
          << "Could not get first template arg from: "
          << name
          << '\n';
    }
    checkDictionaries(arg0, true, level + 2);
    auto arg1 = name_of_template_arg(name, 1);
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
    auto arg0 = name_of_template_arg(name, 0);
    if (arg0.empty()) {
      throw Exception(errors::LogicError, "checkDictionaries: ")
          << "Could not get first template arg from: "
          << name
          << '\n';
    }
    checkDictionaries(arg0, true, level + 2);
    auto arg1 = name_of_template_arg(name, 1);
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
    auto arg0 = name_of_template_arg(name, 0);
    if (arg0.empty()) {
      throw Exception(errors::LogicError, "checkDictionaries: ")
          << "Could not get first template arg from: "
          << name
          << '\n';
    }
    checkDictionaries(arg0, true, level + 2);
    auto arg1 = name_of_template_arg(name, 1);
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
    auto arg0 = name_of_template_arg(name, 0);
    if (arg0.empty()) {
      throw Exception(errors::LogicError, "checkDictionaries: ")
          << "Could not get first template arg from: "
          << name
          << '\n';
    }
    checkDictionaries(arg0, true, level + 2);
    auto arg1 = name_of_template_arg(name, 1);
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
    auto arg0 = name_of_template_arg(name, 0);
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
    auto arg0 = name_of_template_arg(name, 0);
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
    auto arg0 = name_of_template_arg(name, 0);
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
    auto arg0 = name_of_template_arg(name, 0);
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
    auto arg0 = name_of_template_arg(name, 0);
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
    auto arg0 = name_of_template_arg(name, 0);
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
    auto arg0 = name_of_template_arg(name, 0);
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
art::DictionaryChecker::
reportMissingDictionaries()
{
  using namespace std;
  if (missing_types_.empty()) {
    return;
  }
  ostringstream ostr;
  for (auto I = missing_types_.cbegin(), E = missing_types_.cend();
       I != E; ++I) {
    ostr << "     " << cet::demangle_symbol(*I) << "\n";
  }
  resetMissingTypes_();
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
