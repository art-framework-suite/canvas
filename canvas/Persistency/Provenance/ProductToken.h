#ifndef canvas_Persistency_Provenance_ProductToken_h
#define canvas_Persistency_Provenance_ProductToken_h

//==============================================================
// ProductToken is used to enable efficient product lookup via a
// consumes statement given in a module's constructor (e.g.):
//
//   ProductToken<int> nPotsToken_{consumes<int>(inputTag_)};
//   ...
//   auto const& nPotsH = e.getByToken(nPotsToken_); => Handle<int>;
//
// ==============================================================

#include "canvas/Utilities/InputTag.h"

#include <string>

namespace art {

  template <typename T>
  class ProductToken {
  public:
    explicit ProductToken() = default;
    explicit ProductToken(InputTag const& t) : inputTag_{t} {}
    auto const& inputTag() const { return inputTag_; }
  private:
    // For now, the representation is just an InputTag.  For an
    // input-tag that includes a specified process name, the
    // representation could be a ProductID allowing efficient access
    // to the appropriate data.  However, until a mechanism can be
    // determined for combining the needs of specifying a process name
    // vs. not, we will use the InputTag.
    InputTag inputTag_{};
  };

}

#endif /* canvas_Persistency_Provenance_ProductToken_h */

// Local Variables:
// mode: c++
// End:
