#ifndef art_Utilities_fwd_h
#define art_Utilities_fwd_h
////////////////////////////////////////////////////////////////////////
// Fowrard declarations for art/Utilities.
////////////////////////////////////////////////////////////////////////

// These headers are required to forward-declare adequately types and
// contain *only* what is required so to do.
#include "canvas/Utilities/JobMode.h"
#include "canvas/Utilities/Verbosity.h"

namespace art {
  class FirstAbsoluteOrLookupWithDotPolicy;
  typedef long long int HRTimeDiffType; // HRRealTime.h
  typedef unsigned long long int HRTimeType; // HRRealTime.h
  class InputTag;
  class MallocOpts;
  class MallocOptionsSetter; // MallocOpts.h
  class RootHandlers;
}
#endif /* art_Utilities_fwd_h */

// Local Variables:
// mode: c++
// End:
