#ifndef canvas_Persistency_Common_fwd_h
#define canvas_Persistency_Common_fwd_h

// ======================================================================
//
// Forward declarations of types in Persistency/Common
//
// ======================================================================

#include <memory>

namespace art {

  template <typename L, typename R, typename D>
  class Assns;
  template <typename L, typename R>
  class Assns<L, R, void>;
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
  class Wrapper;

} // art

  // ======================================================================

#endif /* canvas_Persistency_Common_fwd_h */

// Local Variables:
// mode: c++
// End:
