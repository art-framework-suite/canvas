#ifndef canvas_Persistency_Provenance_ProductToken_h
#define canvas_Persistency_Provenance_ProductToken_h

//==============================================================
// ProductToken is used to enable efficient product lookup via a
// consumes statement given in a module's constructor (e.g.):
//
//   ProductToken<int> nPotsToken_{consumes<int>(inputTag_)};
//   ...
//   auto const& nPotsH = e.getValidHandle(nPotsToken_); => Handle<int>
//
// The ProductToken class has only private members: access is granted
// via friendship.  The intention is that this class should be
// entirely opaque to the user.
// ==============================================================

#include "canvas/Utilities/InputTag.h"

#include <string>

namespace art {

  template <typename T>
  class ProductToken;

  // Forward declarations needed for granting friendship
  class DataViewImpl;
  class ConsumesRecorder;

  namespace detail {
    template <typename T>
    InputTag input_tag(ProductToken<T> const&);
  }

  template <typename T>
  class ProductToken {

    explicit ProductToken() = default;
    explicit ProductToken(InputTag const& t) : inputTag_{t} {}

    friend class DataViewImpl;
    friend class ConsumesRecorder;
    friend InputTag detail::input_tag<>(ProductToken const&);

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
