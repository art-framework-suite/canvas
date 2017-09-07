#ifndef canvas_IO_Root_Common_AssnsStreamer_h
#define canvas_IO_Root_Common_AssnsStreamer_h

// =====================================================================
// AssnsStreamer: Temporary streamer class until ioread rules are
//                working again.
// =====================================================================

#include "canvas/Persistency/Common/AssnsBase.h"
#include "canvas/Utilities/TypeID.h"

#include "TBuffer.h"
#include "TClassStreamer.h"
#include "TClass.h"
#include "TClassRef.h"

namespace art {
  namespace detail {

    class AssnsStreamer : public TClassStreamer {
      std::string className_;
    public:
      explicit AssnsStreamer(std::string const& className) :
        className_{className}
      {}

      static void init_streamer(std::string className);

      void operator()(TBuffer& R_b, void* objp) {
        static TClassRef cl{TClass::GetClass(className_.c_str())};
        auto obj = reinterpret_cast<detail::AssnsBase*>(objp);
        if (R_b.IsReading()) {
          cl->ReadBuffer(R_b, objp);
          obj->fill_transients();
        }
        else {
          obj->fill_from_transients();
          cl->WriteBuffer(R_b, objp);
        }
      }
    };

    inline void
    AssnsStreamer::init_streamer(std::string className)
    {
      // The streamer affects only the members of the Assns<A,B,void>
      // base class.
      className = name_of_assns_base(className);
      static TClassRef cl{TClass::GetClass(className.c_str())};
      if (cl->GetStreamer() == nullptr) {
        cl->AdoptStreamer(new detail::AssnsStreamer{className});
      }
    }

  }
}
#endif /* canvas_IO_Root_Common_AssnsStreamer_h */

// Local Variables:
// mode: c++
// End:
