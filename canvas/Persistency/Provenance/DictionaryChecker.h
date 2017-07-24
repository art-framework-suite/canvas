#ifndef canvas_Persistency_Provenance_DictionaryChecker_h
#define canvas_Persistency_Provenance_DictionaryChecker_h
#include <set>
#include <string>
#include <vector>

namespace art {
  class DictionaryChecker;
}

/// Manage recursive checking of dictionary information for data products.
class art::DictionaryChecker {
public:

  /// Check dictionaries (optionally recursively) for named type.
  void checkDictionaries(std::string const& name_orig,
                         bool recursive = false,
                         std::size_t level = 0);

  /// Return the sequence of (demangled) types missing dictionaries.
  std::vector<std::string> typesMissingDictionaries();

  /// Report (throwing exception) on all missing dictionaries.
  void reportMissingDictionaries();

private:
  /// Reset missing types list.
  void resetMissingTypes_();
  void checkDictionariesForArg_(std::string const& template_name,
                                std::size_t index,
                                std::size_t level);

  std::set<std::string> checked_names_;
  std::set<std::string> missing_types_;
};

inline
void
art::DictionaryChecker::resetMissingTypes_()
{
  using std::swap;
  std::set<std::string> tmp;
  swap(tmp, missing_types_);
}
#endif /* canvas_Persistency_Provenance_DictionaryChecker_h */


// Local Variables:
// mode: c++
// End:
