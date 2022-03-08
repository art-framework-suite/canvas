#ifndef canvas_Persistency_Common_types_h
#define canvas_Persistency_Common_types_h

#include "canvas/Utilities/TypeID.h"
#include "canvas/Utilities/product_metatype.h"

#include <map>

namespace art {
  using product_typeids_t = std::map<product_metatype, TypeID>;

  enum class Direction : int { Forward = 1, Reverse = -1 };

  template <typename L,
            typename R,
            typename D,
            Direction Dir>
  class const_AssnsIter;

  template <typename L, typename R, typename D>
  class Assns;
  template <typename L, typename R>
  class Assns<L, R, void>;
} // art

#endif /* canvas_Persistency_Common_types_h */

// Local Variables:
// mode: c++
// End:
