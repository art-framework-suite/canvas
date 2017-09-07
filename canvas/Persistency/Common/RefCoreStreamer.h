#ifndef canvas_Persistency_Common_RefCoreStreamer_h
#define canvas_Persistency_Common_RefCoreStreamer_h
// vim: set sw=2 expandtab :

#include "cetlib/exempt_ptr.h"

#include "TClassStreamer.h"

class TBuffer;

namespace art {

class PrincipalBase;

class RefCoreStreamer : public TClassStreamer {

public:

  explicit
  RefCoreStreamer(cet::exempt_ptr<PrincipalBase const> principal = cet::exempt_ptr<PrincipalBase const>());

  void
  setPrincipal(cet::exempt_ptr<PrincipalBase const>);

  virtual
  TClassStreamer*
  Generate() const override;

  void
  operator()(TBuffer&, void*) override;

private:

  cet::exempt_ptr<PrincipalBase const>
  principal_;

};

void
configureRefCoreStreamer(cet::exempt_ptr<PrincipalBase const> principal = cet::exempt_ptr<PrincipalBase const>());

} // namespace art

#endif /* canvas_Persistency_Common_RefCoreStreamer_h */

// Local Variables:
// mode: c++
// End:
