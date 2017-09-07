#include "canvas/Persistency/Common/TriggerResults.h"
// vim: set sw=2 expandtab :

#include "canvas/Persistency/Common/HLTGlobalStatus.h"
#include "canvas/Persistency/Common/traits.h"
#include "fhiclcpp/ParameterSetID.h"

using namespace std;

namespace art {

TriggerResults::
~TriggerResults()
{
}

TriggerResults::
TriggerResults()
  : HLTGlobalStatus()
  , psetID_()
{
}

TriggerResults::
TriggerResults(const HLTGlobalStatus& hlt, const fhicl::ParameterSetID& psetID)
  : HLTGlobalStatus(hlt)
  , psetID_(psetID)
{
}

//TriggerResults&
//TriggerResults::
//operator=(TriggerResults const& rhs)
//{
//  TriggerResults temp(rhs);
//  this->swap(temp);
//  return *this;
//}

fhicl::ParameterSetID const&
TriggerResults::
parameterSetID() const
{
  return psetID_;
}

//void
//TriggerResults::
//swap(TriggerResults& other)
//{
//  this->HLTGlobalStatus::swap(other);
//  psetID_.swap(other.psetID_);
//}

//void
//swap(TriggerResults& lhs, TriggerResults& rhs)
//{
//  lhs.swap(rhs);
//}

} // namespace art

