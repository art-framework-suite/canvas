#include "canvas/Persistency/Common/TriggerResults.h"
// vim: set sw=2 expandtab :

#include "canvas/Persistency/Common/HLTGlobalStatus.h"
#include "canvas/Persistency/Common/traits.h"
#include "fhiclcpp/ParameterSetID.h"

using namespace std;

namespace art {

  TriggerResults::~TriggerResults() {}

  TriggerResults::TriggerResults() : HLTGlobalStatus(), psetid_() {}

  TriggerResults::TriggerResults(const HLTGlobalStatus& hlt,
                                 const fhicl::ParameterSetID& psetID)
    : HLTGlobalStatus(hlt), psetid_(psetID)
  {}

  // TriggerResults&
  // TriggerResults::
  // operator=(TriggerResults const& rhs)
  //{
  //  TriggerResults temp(rhs);
  //  this->swap(temp);
  //  return *this;
  //}

  fhicl::ParameterSetID const&
  TriggerResults::parameterSetID() const
  {
    return psetid_;
  }

  // void
  // TriggerResults::
  // swap(TriggerResults& other)
  //{
  //  this->HLTGlobalStatus::swap(other);
  //  psetid_.swap(other.psetid_);
  //}

  // void
  // swap(TriggerResults& lhs, TriggerResults& rhs)
  //{
  //  lhs.swap(rhs);
  //}

} // namespace art
