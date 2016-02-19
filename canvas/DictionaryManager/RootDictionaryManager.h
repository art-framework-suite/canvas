#ifndef canvas_DictionaryManager_RootDictionaryManager_h
#define canvas_DictionaryManager_RootDictionaryManager_h

// ======================================================================
//
// RootDictionaryManager
//
// ======================================================================

#include "cetlib/LibraryManager.h"
#include <iosfwd>
#include <string>
#include <vector>

// ----------------------------------------------------------------------

namespace art {

  class RootDictionaryManager
  {
  public:
    RootDictionaryManager();

    std::ostream & dumpReflexDictionaryInfo(std::ostream &os) const;

    std::ostream & dumpReflexDictionaryInfo(std::ostream &os,
                               std::string const &libpath) const;

    bool dictIsLoadable(std::string const &path) const;
    bool dictIsLoaded(std::string const &path) const;

  private:
    typedef std::vector<std::string> lib_list_t;
    void loadAllDictionaries();

    cet::LibraryManager dm_; // Dictionaries
    cet::LibraryManager mm_; // Maps
  };  // RootDictionaryManager

}  // art

// ======================================================================

#endif /* canvas_DictionaryManager_RootDictionaryManager_h */

// Local Variables:
// mode: c++
// End:
