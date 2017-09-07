#include "canvas/Persistency/Provenance/BranchDescription.h"
// vim: set sw=2 expandtab :

#include "canvas/Persistency/Provenance/ModuleDescription.h"
#include "canvas/Persistency/Provenance/canonicalProductName.h"
#include "canvas/Utilities/Exception.h"
#include "canvas/Utilities/FriendlyName.h"
#include "canvas/Utilities/WrappedClassName.h"
#include "fhiclcpp/ParameterSetID.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "Compression.h"
#include "TBuffer.h"
#include "TClass.h"
#include "TClassStreamer.h"
#include "TDictAttributeMap.h"

#include <cassert>
#include <cstdlib>
#include <ostream>
#include <sstream>
#include <utility>

using namespace std;

using fhicl::ParameterSetID;

namespace {

// Note: throws
void throwExceptionWithText(char const* txt)
{
  throw art::Exception(art::errors::LogicError)
      << "Problem using an incomplete BranchDescription\n"
      << txt
      << "\nPlease report this error to the ART developers\n";
}

} // unnamed namespace

namespace art {

BranchDescription::
Transients::
~Transients()
{
}

BranchDescription::
Transients::
Transients()
  : branchName_{}
  , wrappedName_{}
  , produced_{false}
  , transient_{false}
  , splitLevel_{}
  , basketSize_{}
  , compression_{invalidCompression}
{
}

BranchDescription::
Transients::
Transients(Transients const& rhs)
  : branchName_{rhs.branchName_}
  , wrappedName_{rhs.wrappedName_}
  , produced_{rhs.produced_}
  , transient_{rhs.transient_}
  , splitLevel_{rhs.splitLevel_}
  , basketSize_{rhs.basketSize_}
  , compression_{rhs.compression_}
{
}

BranchDescription::
Transients::
Transients(Transients&& rhs) noexcept
  : branchName_{move(rhs.branchName_)}
  , wrappedName_{move(rhs.wrappedName_)}
  , produced_{move(rhs.produced_)}
  , transient_{move(rhs.transient_)}
  , splitLevel_{move(rhs.splitLevel_)}
  , basketSize_{move(rhs.basketSize_)}
  , compression_{move(rhs.compression_)}
{
}

BranchDescription::Transients&
BranchDescription::
Transients::
operator=(Transients const& rhs)
{
  if (this != &rhs) {
    branchName_ = rhs.branchName_;
    wrappedName_ = rhs.wrappedName_;
    produced_ = rhs.produced_;
    transient_ = rhs.transient_;
    splitLevel_ = rhs.splitLevel_;
    basketSize_ = rhs.basketSize_;
    compression_ = rhs.compression_;
  }
  return *this;
}

BranchDescription::Transients&
BranchDescription::
Transients::
operator=(Transients&& rhs) noexcept
{
  branchName_ = move(rhs.branchName_);
  wrappedName_ = move(rhs.wrappedName_);
  produced_ = move(rhs.produced_);
  transient_ = move(rhs.transient_);
  splitLevel_ = move(rhs.splitLevel_);
  basketSize_ = move(rhs.basketSize_);
  compression_ = move(rhs.compression_);
  return *this;
}

// Note: throws
BranchDescription::
~BranchDescription()
{
}

// Note: throws
BranchDescription::
BranchDescription()
  : branchType_{InEvent}
  , moduleLabel_{}
  , processName_{}
  , productID_{}
  , producedClassName_{}
  , friendlyClassName_{}
  , productInstanceName_{}
  , psetIDs_{}
  , processConfigurationIDs_{}
  , transients_{}
{
}

// Note: throws
BranchDescription::
BranchDescription(BranchType const bt, TypeLabel const& tl, ModuleDescription const& md)
  : branchType_{bt}
  , moduleLabel_{!tl.emulatedModule().empty() ? tl.emulatedModule() : md.moduleLabel()}
  , processName_{md.processName()}
  , productID_{}
  , producedClassName_{tl.typeID().className()}
  , friendlyClassName_{tl.typeID().friendlyClassName()}
  , productInstanceName_{tl.productInstanceName()}
  , psetIDs_{}
  , processConfigurationIDs_{}
  , transients_{}
{
  guts().produced_ = true;
  psetIDs_.insert(md.parameterSetID());
  processConfigurationIDs_.insert(md.processConfigurationID());
  throwIfInvalid_();
  fluffTransients_();
  initProductID_();
}

// Note: throws
BranchDescription::
BranchDescription(BranchDescription const& rhs)
  : branchType_{rhs.branchType_}
  , moduleLabel_{rhs.moduleLabel_}
  , processName_{rhs.processName_}
  , productID_{rhs.productID_}
  , producedClassName_{rhs.producedClassName_}
  , friendlyClassName_{rhs.friendlyClassName_}
  , productInstanceName_{rhs.productInstanceName_}
  , psetIDs_{rhs.psetIDs_}
  , processConfigurationIDs_{rhs.processConfigurationIDs_}
  , transients_{rhs.transients_}
{
}

// Note: throws
BranchDescription::
BranchDescription(BranchDescription&& rhs)
  : branchType_{move(rhs.branchType_)}
  , moduleLabel_{move(rhs.moduleLabel_)}
  , processName_{move(rhs.processName_)}
  , productID_{move(rhs.productID_)}
  , producedClassName_{move(rhs.producedClassName_)}
  , friendlyClassName_{move(rhs.friendlyClassName_)}
  , productInstanceName_{move(rhs.productInstanceName_)}
  , psetIDs_{move(rhs.psetIDs_)}
  , processConfigurationIDs_{move(rhs.processConfigurationIDs_)}
  , transients_{move(rhs.transients_)}
{
}

// Note: throws
BranchDescription&
BranchDescription::
operator=(BranchDescription const& rhs)
{
  if (this != &rhs) {
    branchType_ = rhs.branchType_;
    moduleLabel_ = rhs.moduleLabel_;
    processName_ = rhs.processName_;
    productID_ = rhs.productID_;
    producedClassName_ = rhs.producedClassName_;
    friendlyClassName_ = rhs.friendlyClassName_;
    productInstanceName_ = rhs.productInstanceName_;
    psetIDs_ = rhs.psetIDs_;
    processConfigurationIDs_ = rhs.processConfigurationIDs_;
    transients_ = rhs.transients_;
  }
  return *this;
}

// Note: throws
BranchDescription&
BranchDescription::
operator=(BranchDescription&& rhs)
{
  branchType_ = move(rhs.branchType_);
  moduleLabel_ = move(rhs.moduleLabel_);
  processName_ = move(rhs.processName_);
  productID_ = move(rhs.productID_);
  producedClassName_ = move(rhs.producedClassName_);
  friendlyClassName_ = move(rhs.friendlyClassName_);
  productInstanceName_ = move(rhs.productInstanceName_);
  psetIDs_ = move(rhs.psetIDs_);
  processConfigurationIDs_ = move(rhs.processConfigurationIDs_);
  transients_ = move(rhs.transients_);
  return *this;
}

string const&
BranchDescription::
moduleLabel() const noexcept
{
  return moduleLabel_;
}

string const&
BranchDescription::
processName() const noexcept
{
  return processName_;
}

string const&
BranchDescription::
producedClassName() const noexcept
{
  return producedClassName_;
}

string const&
BranchDescription::
friendlyClassName() const noexcept
{
  return friendlyClassName_;
}

string const&
BranchDescription::
productInstanceName() const noexcept
{
  return productInstanceName_;
}

bool
BranchDescription::
produced() const noexcept
{
  return guts().produced_;
}

bool
BranchDescription::
transient() const noexcept
{
  return guts().transient_;
}

int
BranchDescription::
splitLevel() const noexcept
{
  return guts().splitLevel_;
}

int
BranchDescription::
basketSize() const noexcept
{
  return guts().basketSize_;
}

int
BranchDescription::
compression() const noexcept
{
  return guts().compression_;
}

set<fhicl::ParameterSetID> const&
BranchDescription::
psetIDs() const noexcept
{
  return psetIDs_;
}

ProductID const&
BranchDescription::
productID() const noexcept
{
  return productID_;
}

BranchType
BranchDescription::
branchType() const noexcept
{
  return branchType_;
}

string const&
BranchDescription::
branchName() const noexcept
{
  return guts().branchName_;
}

string const&
BranchDescription::
wrappedName() const noexcept
{
  return guts().wrappedName_;
}

// Note: throws
void
BranchDescription::
initProductID_()
{
  if (!transientsFluffed_()) {
    return;
  }
  if (!productID_.isValid()) {
    productID_.setID(guts().branchName_);
  }
}

// Note: throws
void
BranchDescription::
fluffTransients_() const
{
  if (transientsFluffed_()) {
    return;
  }
  transients_.get().branchName_ = canonicalProductName(friendlyClassName(), moduleLabel(), productInstanceName(), processName());
  {
    auto pcl = TClass::GetClass(producedClassName().c_str());
    if (pcl == nullptr) {
      //FIXME: This is ok when the produced class is a fundamental type.
      //throw Exception(errors::DictionaryNotFound, "NoTClass")
      //    << "BranchDescription::fluffTransients() could not get TClass for "
      //    "producedClassName: "
      //    << producedClassName()
      //    << '\n';
    }
    else {
      auto am = pcl->GetAttributeMap();
      if (am && am->HasKey("persistent") &&
          am->GetPropertyAsString("persistent") == string("false")) {
        mf::LogWarning("TransientBranch")
            << "BranchDescription::fluffTransients() called for the "
            "non-persistable\n"
            << "entity: "
            << friendlyClassName()
            << ". Please check your experiment's policy\n"
            << "on the advisability of such products.\n";
        transients_.get().transient_ = true;
      }
    }
  }
  transients_.get().wrappedName_ = wrappedClassName(producedClassName());
  transients_.get().splitLevel_ = invalidSplitLevel;
  transients_.get().basketSize_ = invalidBasketSize;
  transients_.get().compression_ = invalidCompression;
  auto wcl = TClass::GetClass(transients_.get().wrappedName_.c_str());
  if (wcl == nullptr) {
    //FIXME: This is ok when the produced class is a fundamental type.
    //throw Exception(errors::DictionaryNotFound, "NoTClass")
    //    << "BranchDescription::fluffTransients() could not get TClass for "
    //    << "wrappedName: "
    //    << transients_.get().wrappedName_
    //    << '\n';
    return;
  }
  auto wam = wcl->GetAttributeMap();
  if (wam == nullptr) {
    // No attributes specified, all done.
    return;
  }
  if (wam->HasKey("splitLevel")) {
    transients_.get().splitLevel_ =
      strtol(wam->GetPropertyAsString("splitLevel"), 0, 0);
    if (transients_.get().splitLevel_ < 0) {
      throw Exception(errors::Configuration, "IllegalSplitLevel")
          << "' An illegal ROOT split level of "
          << transients_.get().splitLevel_
          << " is specified for class "
          << transients_.get().wrappedName_
          << ".'\n";
    }
    ++transients_.get().splitLevel_; //Compensate for wrapper
  }
  if (wam->HasKey("basketSize")) {
    transients_.get().basketSize_ =
      strtol(wam->GetPropertyAsString("basketSize"), 0, 0);
    if (transients_.get().basketSize_ <= 0) {
      throw Exception(errors::Configuration, "IllegalBasketSize")
          << "' An illegal ROOT basket size of "
          << transients_.get().basketSize_
          << " is specified for class "
          << transients_.get().wrappedName_
          << "'.\n";
    }
  }
  if (wam->HasKey("compression")) {
    // FIXME: We need to check for a parsing error from the strtol() here!
    int compression = strtol(wam->GetPropertyAsString("compression"), 0, 0);
    if (compression < 0) {
      throw Exception(errors::Configuration, "IllegalCompression")
          << "' An illegal ROOT compression of "
          << compression
          << " is specified for class "
          << transients_.get().wrappedName_
          << "'.\n";
    }
    int algorithm = compression / 100;
    int level = compression % 100;
    if (algorithm >= ROOT::kUndefinedCompressionAlgorithm) {
      throw Exception(errors::Configuration, "IllegalCompressionAlgorithm")
          << "' An illegal ROOT compression algorithm of "
          << algorithm
          << " is specified for class "
          << transients_.get().wrappedName_
          << "'.\n";
    }
    if (level > 9) {
      throw Exception(errors::Configuration, "IllegalCompressionLevel")
          << "' An illegal ROOT compression level of "
          << algorithm
          << " is specified for class "
          << transients_.get().wrappedName_
          << "'.  The compression level must between 0 and 9 inclusive.\n";
    }
    transients_.get().compression_ = compression;
  }
}

// Note: throws
ParameterSetID const&
BranchDescription::
psetID() const
{
  assert(!psetIDs().empty());
  if (psetIDs().size() != 1) {
    throw Exception(errors::Configuration, "AmbiguousProvenance")
        << "Your application requires all events on Branch '"
        << guts().branchName_
        << "'\n to have the same provenance. This file has events with "
        "mixed provenance\n"
        << "on this branch.  Use a different input file.\n";
  }
  return *psetIDs().begin();
}

// Note: throws
void
BranchDescription::
merge(BranchDescription const& other)
{
  psetIDs_.insert(other.psetIDs().begin(), other.psetIDs().end());
  processConfigurationIDs_.insert(other.processConfigurationIDs().begin(),
                                  other.processConfigurationIDs().end());
  if (guts().splitLevel_ == invalidSplitLevel) {
    guts().splitLevel_ = other.guts().splitLevel_;
  }
  if (guts().basketSize_ == invalidBasketSize) {
    guts().basketSize_ = other.guts().basketSize_;
  }
  // FIXME: This is probably wrong! We are going from defaulted compression
  //        to a possibly different compression, bad.
  if (guts().compression_ == invalidCompression) {
    guts().compression_ = other.guts().compression_;
  }
}

// Note: throws
void
BranchDescription::
write(ostream& os) const
{
  os << "Branch Type = " << branchType_ << endl;
  os << "Process Name = " << processName() << endl;
  os << "ModuleLabel = " << moduleLabel() << endl;
  os << "Product ID = " << productID() << '\n';
  os << "Class Name = " << producedClassName() << '\n';
  os << "Friendly Class Name = " << friendlyClassName() << '\n';
  os << "Product Instance Name = " << productInstanceName() << endl;
}

// Note: throws
void
BranchDescription::
swap(BranchDescription& other)
{
  std::swap(branchType_, other.branchType_);
  std::swap(moduleLabel_, other.moduleLabel_);
  std::swap(processName_, other.processName_);
  std::swap(productID_, other.productID_);
  std::swap(producedClassName_, other.producedClassName_);
  std::swap(friendlyClassName_, other.friendlyClassName_);
  std::swap(productInstanceName_, other.productInstanceName_);
  std::swap(psetIDs_, other.psetIDs_);
  std::swap(processConfigurationIDs_, other.processConfigurationIDs_);
  std::swap(transients_, other.transients_);
}

// Note: throws
void
BranchDescription::
throwIfInvalid_() const
{
  constexpr char underscore('_');
  if (transientsFluffed_()) {
    return;
  }
  if (branchType_ >= NumBranchTypes) {
    throwExceptionWithText("Illegal BranchType detected");
  }
  if (moduleLabel_.empty()) {
    throwExceptionWithText("Module label is not allowed to be empty");
  }
  if (processName_.empty()) {
    throwExceptionWithText("Process name is not allowed to be empty");
  }
  if (producedClassName_.empty()) {
    throwExceptionWithText("Full class name is not allowed to be empty");
  }
  if (friendlyClassName_.empty()) {
    throwExceptionWithText("Friendly class name is not allowed to be empty");
  }
  if (friendlyClassName_.find(underscore) != string::npos) {
    throw Exception(errors::LogicError, "IllegalCharacter")
        << "Class name '"
        << friendlyClassName()
        << "' contains an underscore ('_'), which is illegal in the "
        << "name of a product.\n";
  }
  if (moduleLabel_.find(underscore) != string::npos) {
    throw Exception(errors::Configuration, "IllegalCharacter")
        << "Module label '"
        << moduleLabel()
        << "' contains an underscore ('_'), which is illegal in a "
        << "module label.\n";
  }
  if (productInstanceName_.find(underscore) != string::npos) {
    throw Exception(errors::Configuration, "IllegalCharacter")
        << "Product instance name '"
        << productInstanceName()
        << "' contains an underscore ('_'), which is illegal in a "
        << "product instance name.\n";
  }
  if (processName_.find(underscore) != string::npos) {
    throw Exception(errors::Configuration, "IllegalCharacter")
        << "Process name '"
        << processName()
        << "' contains an underscore ('_'), which is illegal in a "
        << "process name.\n";
  }
}

bool
BranchDescription::
transientsFluffed_() const noexcept
{
  return !guts().branchName_.empty();
}

bool
BranchDescription::
isPsetIDUnique() const noexcept
{
  return psetIDs().size() == 1;
}

set<ProcessConfigurationID> const&
BranchDescription::
processConfigurationIDs() const noexcept
{
  return processConfigurationIDs_;
}

BranchDescription::Transients&
BranchDescription::
guts() noexcept
{
  return transients_.get();
}

BranchDescription::Transients const&
BranchDescription::
guts() const noexcept
{
  return transients_.get();
}

bool
operator<(BranchDescription const& a, BranchDescription const& b)
{
  if (a.processName() < b.processName()) {
    return true;
  }
  if (b.processName() < a.processName()) {
    return false;
  }
  if (a.producedClassName() < b.producedClassName()) {
    return true;
  }
  if (b.producedClassName() < a.producedClassName()) {
    return false;
  }
  if (a.friendlyClassName() < b.friendlyClassName()) {
    return true;
  }
  if (b.friendlyClassName() < a.friendlyClassName()) {
    return false;
  }
  if (a.productInstanceName() < b.productInstanceName()) {
    return true;
  }
  if (b.productInstanceName() < a.productInstanceName()) {
    return false;
  }
  if (a.moduleLabel() < b.moduleLabel()) {
    return true;
  }
  if (b.moduleLabel() < a.moduleLabel()) {
    return false;
  }
  if (a.branchType() < b.branchType()) {
    return true;
  }
  if (b.branchType() < a.branchType()) {
    return false;
  }
  if (a.productID() < b.productID()) {
    return true;
  }
  if (b.productID() < a.productID()) {
    return false;
  }
  if (a.psetIDs() < b.psetIDs()) {
    return true;
  }
  if (b.psetIDs() < a.psetIDs()) {
    return false;
  }
  if (a.processConfigurationIDs() < b.processConfigurationIDs()) {
    return true;
  }
  if (b.processConfigurationIDs() < a.processConfigurationIDs()) {
    return false;
  }
  return false;
}

bool
combinable(BranchDescription const& a, BranchDescription const& b)
{
  return
    (a.branchType() == b.branchType()) &&
    (a.processName() == b.processName()) &&
    (a.producedClassName() == b.producedClassName()) &&
    (a.friendlyClassName() == b.friendlyClassName()) &&
    (a.productInstanceName() == b.productInstanceName()) &&
    (a.moduleLabel() == b.moduleLabel()) &&
    (a.productID() == b.productID());
}

bool
operator==(BranchDescription const& a, BranchDescription const& b)
{
  return combinable(a, b) && (a.psetIDs() == b.psetIDs()) && (a.processConfigurationIDs() == b.processConfigurationIDs());
}

class detail::BranchDescriptionStreamer : public TClassStreamer {

public:

  TClassStreamer*
  Generate() const override;

  void
  operator()(TBuffer&, void* objp) override;

};

TClassStreamer*
detail::
BranchDescriptionStreamer::
Generate() const
{
  return new detail::BranchDescriptionStreamer{*this};
}

void
detail::
BranchDescriptionStreamer::
operator()(TBuffer& R_b, void* objp)
{
  static TClassRef cl{TClass::GetClass(typeid(BranchDescription))};
  auto obj = reinterpret_cast<BranchDescription*>(objp);
  if (R_b.IsReading()) {
    cl->ReadBuffer(R_b, obj);
    obj->fluffTransients_();
  }
  else {
    cl->WriteBuffer(R_b, obj);
  }
}

void
detail::
setBranchDescriptionStreamer()
{
  static TClassRef cl{TClass::GetClass(typeid(BranchDescription))};
  if (cl->GetStreamer() == nullptr) {
    cl->AdoptStreamer(new BranchDescriptionStreamer);
  }
}

ostream&
operator<<(ostream& os, BranchDescription const& p)
{
  p.write(os);
  return os;
}

} // namespace art

