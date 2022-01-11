#ifndef canvas_Persistency_Provenance_History_h
#define canvas_Persistency_Provenance_History_h
// vim: set sw=2 expandtab :

//
// Class History represents the processing history of a single Event.
// It includes ordered sequences of elements, each of which contains
// information about a specific 'process' through which the Event has
// passed, with earlier processes at the beginning of the sequence.
//

#include "canvas/Persistency/Provenance/EventSelectionID.h"
#include "canvas/Persistency/Provenance/ProcessHistoryID.h"

namespace art {

  class History {
  public:
    ProcessHistoryID const& processHistoryID() const noexcept;
    void setProcessHistoryID(ProcessHistoryID const& phid);

  private:
    EventSelectionIDVector eventSelections_;
    ProcessHistoryID processHistoryID_;
  };

} // namespace art

#endif /* canvas_Persistency_Provenance_History_h */

// Local Variables:
// mode: c++
// End:
