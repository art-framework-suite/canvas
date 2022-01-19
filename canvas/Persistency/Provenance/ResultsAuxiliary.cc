#include "canvas/Persistency/Provenance/ResultsAuxiliary.h"
// vim: set sw=2 expandtab :

#include <ostream>

void
art::ResultsAuxiliary::write(std::ostream& os) const
{
  os << "Process History ID = " << processHistoryID_ << std::endl;
}
