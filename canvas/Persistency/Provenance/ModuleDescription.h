#ifndef canvas_Persistency_Provenance_ModuleDescription_h
#define canvas_Persistency_Provenance_ModuleDescription_h
// vim: set sw=2 expandtab :

#include "canvas/Persistency/Provenance/ModuleDescriptionID.h"
#include "canvas/Persistency/Provenance/ProcessConfiguration.h"
#include "canvas/Persistency/Provenance/ProcessConfigurationID.h"
#include "fhiclcpp/ParameterSetID.h"
#include <iosfwd>
#include <string>

namespace art {

  class ModuleDescription {

  public:
    static ModuleDescriptionID getUniqueID();

    static constexpr ModuleDescriptionID
    invalidID()
    {
      return std::numeric_limits<ModuleDescriptionID>::max();
    }

  public:
    ~ModuleDescription();

    explicit ModuleDescription();

    explicit ModuleDescription(fhicl::ParameterSetID parameterSetID,
                               std::string const& modName,
                               std::string const& modLabel,
                               int moduleThreadingType,
                               ProcessConfiguration pc,
                               ModuleDescriptionID id = getUniqueID());

    ModuleDescriptionID id() const;

    void write(std::ostream& os) const;

    fhicl::ParameterSetID const& parameterSetID() const;

    std::string const& moduleName() const;

    std::string const& moduleLabel() const;

    int moduleThreadingType() const;

    ProcessConfiguration const& processConfiguration() const;

    ProcessConfigurationID const processConfigurationID() const;

    std::string const& processName() const;

    std::string const& releaseVersion() const;

    fhicl::ParameterSetID const& mainParameterSetID() const;

    bool operator<(ModuleDescription const& rh) const;

    bool operator==(ModuleDescription const& rh) const;

    bool operator!=(ModuleDescription const& rh) const;

  private:
    // ID of parameter set of the creator
    fhicl::ParameterSetID parameterSetID_{};

    // The class name of the creator
    std::string moduleName_{};

    // A human friendly string that uniquely identifies the EDProducer
    // and becomes part of the identity of a product that it produces
    std::string moduleLabel_{};

    // What kind of multi-threading the module supports.
    int moduleThreadingType_{};

    // The process configuration.
    ProcessConfiguration processConfiguration_{};

    // Unique ID.
    ModuleDescriptionID id_{};
  };

  std::ostream& operator<<(std::ostream& os, ModuleDescription const& p);

} // namespace art

#endif /* canvas_Persistency_Provenance_ModuleDescription_h */

// Local Variables:
// mode: c++
// End:
