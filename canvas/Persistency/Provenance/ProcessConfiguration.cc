#include "canvas/Persistency/Provenance/ProcessConfiguration.h"
// vim: set sw=2 expandtab :

#include "cetlib/MD5Digest.h"

#include <ostream>
#include <sstream>

using namespace std;

namespace art {

ProcessConfiguration::
~ProcessConfiguration()
{
}

// Note: Cannot be noexcept because the psetid ctor can throw.
ProcessConfiguration::
ProcessConfiguration()
  : processName_{}
    // Note: threading: The psetid ctor can throw!
  , parameterSetID_{}
  , releaseVersion_{}
{
}

// Note: Cannot be noexcept because the psetid ctor can throw.
ProcessConfiguration::
ProcessConfiguration(string const& procName, fhicl::ParameterSetID const& pSetID, ReleaseVersion const& relVersion)
  : processName_{procName}
    // Note: threading: The psetid ctor can throw!
  , parameterSetID_{pSetID}
  , releaseVersion_{relVersion}
{
}

// Note: Cannot be noexcept because the psetid ctor can throw.
ProcessConfiguration::
ProcessConfiguration(ProcessConfiguration const& rhs)
  : processName_{rhs.processName_}
    // Note: threading: The psetid ctor can throw!
  , parameterSetID_{rhs.parameterSetID_}
  , releaseVersion_{rhs.releaseVersion_}
{
}

// Note: Cannot be noexcept because the psetid ctor can throw.
ProcessConfiguration::
ProcessConfiguration(ProcessConfiguration&& rhs)
  : processName_{move(rhs.processName_)}
    // Note: threading: The psetid ctor can throw!
  , parameterSetID_{move(rhs.parameterSetID_)}
  , releaseVersion_{move(rhs.releaseVersion_)}
{
}

ProcessConfiguration&
ProcessConfiguration::
operator=(ProcessConfiguration const& rhs)
{
  if (this != &rhs) {
    processName_ = rhs.processName_;
    parameterSetID_ = rhs.parameterSetID_;
    releaseVersion_ = rhs.releaseVersion_;
  }
  return *this;
}

ProcessConfiguration&
ProcessConfiguration::
operator=(ProcessConfiguration&& rhs)
{
  processName_ = move(rhs.processName_);
  parameterSetID_ = move(rhs.parameterSetID_);
  releaseVersion_ = move(rhs.releaseVersion_);
  return *this;
}

string const&
ProcessConfiguration::
processName() const
{
  return processName_;
}

fhicl::ParameterSetID const&
ProcessConfiguration::
parameterSetID() const
{
  return parameterSetID_;
}

ReleaseVersion const&
ProcessConfiguration::
releaseVersion() const
{
  return releaseVersion_;
}

ProcessConfigurationID
ProcessConfiguration::
id() const
{
  ostringstream oss;
  oss << *this;
  string stringrep = oss.str();
  cet::MD5Digest md5alg(stringrep);
  return ProcessConfigurationID(md5alg.digest().toString());
}

bool
operator==(ProcessConfiguration const& a, ProcessConfiguration const& b)
{
  return (a.processName() == b.processName()) &&
         (a.parameterSetID() == b.parameterSetID()) &&
         (a.releaseVersion() == b.releaseVersion());
}

bool
operator!=(ProcessConfiguration const& a, ProcessConfiguration const& b)
{
  return !(a == b);
}

bool
operator<(ProcessConfiguration const& a, ProcessConfiguration const& b)
{
  if (a.processName() < b.processName()) {
    return true;
  }
  if (b.processName() < a.processName()) {
    return false;
  }
  if (a.parameterSetID() < b.parameterSetID()) {
    return true;
  }
  if (b.parameterSetID() < a.parameterSetID()) {
    return false;
  }
  if (a.releaseVersion() < b.releaseVersion()) {
    return true;
  }
  if (b.releaseVersion() < a.releaseVersion()) {
    return false;
  }
  return false;
}

ostream&
operator<<(ostream& os, ProcessConfiguration const& pc)
{
  os
    << pc.processName()
    << ' '
    << pc.parameterSetID()
    << ' '
    << pc.releaseVersion()
    << ' ';
  return os;
}

} // namespace art

