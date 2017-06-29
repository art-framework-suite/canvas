#include "canvas/Persistency/Provenance/ProcessConfiguration.h"
#include "cetlib/MD5Digest.h"

#include <ostream>
#include <sstream>
#include <tuple>

namespace art {

  ProcessConfigurationID
  ProcessConfiguration::id() const
  {
    // This implementation is ripe for optimization.
    std::ostringstream oss;
    oss << *this;
    std::string stringrep = oss.str();
    cet::MD5Digest md5alg(stringrep);
    return ProcessConfigurationID(md5alg.digest().toString());
  }

  bool operator<(ProcessConfiguration const& a, ProcessConfiguration const& b) {
    return std::tie(a.processName_, a.parameterSetID_, a.releaseVersion_, a.passID_) <
           std::tie(b.processName_, b.parameterSetID_, b.releaseVersion_, b.passID_);
  }

  bool operator==(ProcessConfiguration const& a, ProcessConfiguration const& b) {
    return std::tie(a.processName_, a.parameterSetID_, a.releaseVersion_, a.passID_) ==
           std::tie(b.processName_, b.parameterSetID_, b.releaseVersion_, b.passID_);
  }

  std::ostream&
  operator<< (std::ostream& os, ProcessConfiguration const& pc) {
    os << pc.processName_ << ' '
       << pc.parameterSetID_ << ' '
       << pc.releaseVersion_ << ' '
       << pc.passID_;
    return os;
  }
}
