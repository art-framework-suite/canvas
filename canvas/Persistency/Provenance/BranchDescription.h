#ifndef canvas_Persistency_Provenance_BranchDescription_h
#define canvas_Persistency_Provenance_BranchDescription_h
// vim: set sw=2 expandtab :

//
//  The full description of data product branch.
//

#include "canvas/Persistency/Provenance/BranchType.h"
#include "canvas/Persistency/Provenance/ProcessConfigurationID.h"
#include "canvas/Persistency/Provenance/ProductID.h"
#include "canvas/Persistency/Provenance/ProvenanceFwd.h"
#include "canvas/Persistency/Provenance/Transient.h"
#include "canvas/Persistency/Provenance/TypeLabel.h"
#include "fhiclcpp/ParameterSetID.h"

#include <iosfwd>
#include <set>
#include <string>

namespace art {


namespace detail {

  class BranchDescriptionStreamer;

  void
  setBranchDescriptionStreamer();

} // namespace detail

class BranchDescription {

  friend bool combinable(BranchDescription const&, BranchDescription const&);
  friend bool operator<(BranchDescription const&, BranchDescription const&);
  friend bool operator==(BranchDescription const&, BranchDescription const&);

  friend class detail::BranchDescriptionStreamer;

public: // TYPES

  static int constexpr invalidSplitLevel = -1;
  static int constexpr invalidBasketSize = 0;
  static int constexpr invalidCompression = -1;

  struct Transients {

    ~Transients();

    Transients();

    Transients(Transients const&);

    Transients(Transients&&) noexcept;

    Transients&
    operator=(Transients const&);

    Transients&
    operator=(Transients&&) noexcept;

    // The branch name, which is currently derivable fron the other attributes.
    std::string
    branchName_{};

    // The wrapped class name, which is currently derivable fron the other attributes.
    std::string
    wrappedName_{};

    // Was this branch produced in this process
    // rather than in a previous process
    bool
    produced_{false};

    // Is the class of the branch marked as transient
    // in the data dictionary
    bool
    transient_{false};

    // The split level of the branch, as marked
    // in the data dictionary.
    int
    splitLevel_{};

    // The basket size of the branch, as marked
    // in the data dictionary.
    int
    basketSize_{};

    // The compression of the branch, as marked
    // in the data dictionary.
    int
    compression_{invalidCompression};

  };

public:

  ~BranchDescription();

  BranchDescription();

  BranchDescription(BranchType const, TypeLabel const&, ModuleDescription const&);

  BranchDescription(BranchDescription const&);

  BranchDescription(BranchDescription&&);

  BranchDescription&
  operator=(BranchDescription const&);

  BranchDescription&
  operator=(BranchDescription&&);

public:

  std::string const&
  moduleLabel() const noexcept;

  std::string const&
  processName() const noexcept;

  std::string const&
  producedClassName() const noexcept;

  std::string const&
  friendlyClassName() const noexcept;

  std::string const&
  productInstanceName() const noexcept;

  bool
  produced() const noexcept;

  bool
  transient() const noexcept;

  int
  splitLevel() const noexcept;

  int
  basketSize() const noexcept;

  int
  compression() const noexcept;

  std::set<fhicl::ParameterSetID> const&
  psetIDs() const noexcept;

  ProductID const&
  productID() const noexcept;

  BranchType
  branchType() const noexcept;

  std::string const&
  branchName() const noexcept;

  std::string const&
  wrappedName() const noexcept;

  void
  merge(BranchDescription const&);

  void
  swap(BranchDescription&);

  void
  write(std::ostream&) const;

private:

  fhicl::ParameterSetID const&
  psetID() const;

  void
  initProductID_();

  void
  fluffTransients_() const;

  bool
  transientsFluffed_() const noexcept;

  bool
  isPsetIDUnique() const noexcept;

  std::set<ProcessConfigurationID> const&
  processConfigurationIDs() const noexcept;

  Transients&
  guts() noexcept;

  Transients const&
  guts() const noexcept;

  void
  throwIfInvalid_() const;

private:

  // What tree is the branch in?
  BranchType
  branchType_{InEvent};

  // A human-friendly string that uniquely identifies the EDProducer
  // and becomes part of the identity of a product that it produces
  std::string
  moduleLabel_{};

  // the physical process that this program was part of (e.g. production)
  std::string
  processName_{};

  // An ID uniquely identifying the product
  ProductID
  productID_{};

  // the full name of the type of product this is
  std::string
  producedClassName_{};

  // a readable name of the type of product this is
  std::string
  friendlyClassName_{};

  // a user-supplied name to distinguish multiple products of the same type
  // that are produced by the same producer
  std::string
  productInstanceName_{};

  // ID's of parameter set of the creators of products
  // on this branch
  std::set<fhicl::ParameterSetID>
  psetIDs_{};

  // ID's of process configurations for products
  // on this branch
  std::set<ProcessConfigurationID>
  processConfigurationIDs_{};

  // The things we do not want saved to disk.
  mutable
  Transient<Transients>
  transients_{};

};

std::ostream&
operator<<(std::ostream&, BranchDescription const&);

bool
operator<(BranchDescription const&, BranchDescription const&);

bool
operator==(BranchDescription const&, BranchDescription const&);

bool
combinable(BranchDescription const&, BranchDescription const&);

} // namespace art

#endif /* canvas_Persistency_Provenance_BranchDescription_h */

// Local Variables:
// mode: c++
// End:
