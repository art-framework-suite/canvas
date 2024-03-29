#ifndef canvas_Persistency_Provenance_ProductToken_h
#define canvas_Persistency_Provenance_ProductToken_h

//==============================================================
// ProductToken and ViewToken are used to enable efficient product
// lookup via a consumes statement given in a module's constructor
// (e.g.):
//
//   ProductToken<int> nPotsToken_{consumes<int>(inputTag_)};
//   ...
//   auto const& nPotsH = e.getValidHandle(nPotsToken_); => ValidHandle<int>
//
// The ProductToken and ViewToken classes have only private members:
// access is granted via friendship.  The intention is that these
// classes should be entirely opaque to the user.
// ==============================================================

#include "canvas/Utilities/InputTag.h"

#include <ostream>
#include <string>

namespace gallery {
  class Event;
}

namespace art {

  template <typename T>
  class ProductToken;

  template <typename T>
  class ViewToken;

  namespace detail {
    template <typename ProdA, typename ProdB, typename Data>
    struct safe_input_tag;
  }

  template <typename T>
  class ProductToken {
  public:
    using product_type = T;

    friend std::ostream&
    operator<<(std::ostream& os, ProductToken const& tok)
    {
      os << tok.inputTag_;
      return os;
    }

    static ProductToken<T>
    invalid()
    {
      return ProductToken<T>{};
    }
    explicit ProductToken() = default;
    explicit ProductToken(InputTag const& t) : inputTag_{t} {}

    InputTag const&
    inputTag() const
    {
      return inputTag_;
    }

  private:
    template <typename ProdA, typename ProdB, typename Data>
    friend struct detail::safe_input_tag;

    // For now, the representation is just an InputTag.  For an
    // input-tag that includes a specified process name, the
    // representation could be a ProductID allowing efficient access
    // to the appropriate data.  However, until a mechanism can be
    // determined for combining the needs of specifying a process name
    // vs. not, we will use the InputTag.

    InputTag inputTag_{};
  };

  template <typename Element>
  class ViewToken {
  public:
    using element_type = Element;

    static ViewToken<Element>
    invalid()
    {
      return ViewToken<Element>{};
    }
    explicit ViewToken() = default;
    explicit ViewToken(InputTag const& t) : inputTag_{t} {}

    InputTag const&
    inputTag() const
    {
      return inputTag_;
    }

    friend std::ostream&
    operator<<(std::ostream& os, ViewToken const& tok)
    {
      os << tok.inputTag_;
      return os;
    }

  private:
    // See notes in ProductToken re. the representation.
    InputTag inputTag_{};
  };
}

#endif /* canvas_Persistency_Provenance_ProductToken_h */

// Local Variables:
// mode: c++
// End:
