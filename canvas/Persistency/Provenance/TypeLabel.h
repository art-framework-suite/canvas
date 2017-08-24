#ifndef canvas_Persistency_Provenance_TypeLabel_h
#define canvas_Persistency_Provenance_TypeLabel_h

#include <memory>
#include <string>
#include "canvas/Utilities/TypeID.h"

namespace art {

  class TypeLabel {
  public:

    TypeLabel(TypeID const& itemtype,
              std::string const& instanceName,
              bool const supportsView);

    TypeLabel(TypeID const& itemtype,
              std::string const& instanceName,
              bool const supportsView,
              std::string emulatedModule);

    auto const& typeID() const { return typeID_; }
    std::string className() const { return typeID_.className(); }
    std::string friendlyClassName() const { return typeID_.friendlyClassName(); }
    std::string const& emulatedModule() const;
    std::string const& productInstanceName() const { return productInstanceName_; }
    bool hasEmulatedModule() const { return static_cast<bool>(emulatedModule_); }
    bool supportsView() const { return supportsView_; }

  private:
    TypeID      typeID_;
    std::string productInstanceName_;
    bool        supportsView_;
    std::shared_ptr<std::string> emulatedModule_{nullptr}; // shared so TypeLabel is copyable
    friend bool operator<(TypeLabel const& a, TypeLabel const& b);
  };

  bool operator<(TypeLabel const& a, TypeLabel const& b);
}



#endif /* canvas_Persistency_Provenance_TypeLabel_h */

// Local Variables:
// mode: c++
// End:
