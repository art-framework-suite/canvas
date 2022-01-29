#ifndef canvas_Persistency_Common_fwd_h
#define canvas_Persistency_Common_fwd_h

// ======================================================================
//
// Forward declarations of types in Persistency/Common
//
// ======================================================================

#include "canvas/Persistency/Common/types.h"

namespace art {
  namespace detail {
    class AssnsBase;
  }

  class EDProduct;
  class EDProductGetter;
  class GroupQueryResult;
  class HLTGlobalStatus;
  class HLTPathStatus;
  class OutputHandle;
  template <typename T>
  class Ptr;
  template <typename T>
  class PtrVector;
  class PtrVectorBase;
  class ProductID;
  class RefCore;
  template <typename T>
  class Sampled;
  class TriggerResults;
  template <typename T>
  class Wrapper;
  struct WrappedTypeID;
} // art

// ======================================================================

#endif /* canvas_Persistency_Common_fwd_h */

// Local Variables:
// mode: c++
// End:
