#ifndef canvas_Persistency_Provenance_TypeLabel_h
#define canvas_Persistency_Provenance_TypeLabel_h
// vim: set sw=2 expandtab :

#include <string>
#include "canvas/Utilities/TypeID.h"

namespace art {

class TypeLabel {

  friend bool operator<(TypeLabel const& a, TypeLabel const& b);

public: // MEMBER FUNCTIONS -- Special Member Functions

  TypeLabel(TypeID const& itemtype, std::string const& instanceName, std::string const& emulatedMod = {});

public: // MEMBER FUNCTIONS

  TypeID const&
  typeID() const;

  //std::string
  //className() const;

  //std::string
  //friendlyClassName() const;

  std::string const&
  productInstanceName() const;

  std::string const&
  emulatedModule() const;

  //bool
  //hasEmulatedModule() const;

private: // MEMBER DATA

  TypeID
  typeID_;

  std::string
  productInstanceName_;

  std::string
  emulatedModule_;

};

} // namespace art

#endif /* canvas_Persistency_Provenance_TypeLabel_h */

// Local Variables:
// mode: c++
// End:
