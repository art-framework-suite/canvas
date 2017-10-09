#ifndef canvas_Persistency_Provenance_ProcessConfiguration_h
#define canvas_Persistency_Provenance_ProcessConfiguration_h
// vim: set sw=2 expandtab :

#include "canvas/Persistency/Provenance/ProcessConfigurationID.h"
#include "canvas/Persistency/Provenance/ReleaseVersion.h"
#include "fhiclcpp/ParameterSetID.h"

#include <iosfwd>
#include <string>

namespace art {

  class ProcessConfiguration {

  public:
    ~ProcessConfiguration();

    // Note: Cannot be noexcept because the psetid ctor can throw.
    ProcessConfiguration();

    // Note: Cannot be noexcept because the psetid ctor can throw.
    ProcessConfiguration(ProcessConfiguration const&);

    // Note: Cannot be noexcept because the psetid ctor can throw.
    ProcessConfiguration(ProcessConfiguration&&);

    ProcessConfiguration& operator=(ProcessConfiguration const&);

    ProcessConfiguration& operator=(ProcessConfiguration&&);

    // Note: Cannot be noexcept because the psetid ctor can throw.
    ProcessConfiguration(std::string const& name,
                         fhicl::ParameterSetID const&,
                         ReleaseVersion const&);

  public:
    std::string const& processName() const;

    fhicl::ParameterSetID const& parameterSetID() const;

    ReleaseVersion const& releaseVersion() const;

    ProcessConfigurationID id() const;

  private:
    std::string processName_{};
    // Note: threading: The psetid constructor can throw!
    fhicl::ParameterSetID parameterSetID_{};
    // Note: This is just a std::string!
    ReleaseVersion releaseVersion_{};
  };

  bool operator<(ProcessConfiguration const& a, ProcessConfiguration const& b);

  bool operator==(ProcessConfiguration const& a, ProcessConfiguration const& b);

  bool operator!=(ProcessConfiguration const& a, ProcessConfiguration const& b);

  std::ostream& operator<<(std::ostream& os, ProcessConfiguration const& pc);

} // namespace art

#endif /* canvas_Persistency_Provenance_ProcessConfiguration_h */

// Local Variables:
// mode: c++
// End:
