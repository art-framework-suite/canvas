include(CetMake)

find_package(Boost COMPONENTS headers REQUIRED PUBLIC)
find_package(Boost PRIVATE COMPONENTS headers date_time REQUIRED)
find_package(CLHEP PRIVATE COMPONENTS Matrix Vector REQUIRED)
find_package(Range-v3 REQUIRED PUBLIC)
find_package(cetlib REQUIRED PUBLIC)
find_package(cetlib_except REQUIRED PUBLIC)
find_package(fhiclcpp REQUIRED PUBLIC)
find_package(hep_concurrency REQUIRED PUBLIC)
find_package(messagefacility PRIVATE REQUIRED)

cet_make_library(LIBRARY_NAME canvas
  SOURCE
    ${CMAKE_CURRENT_BINARY_DIR}/Version/GetReleaseVersion.cc
    Persistency/Common/EDProduct.cc
    Persistency/Common/EDProductGetter.cc
    Persistency/Common/HLTGlobalStatus.cc
    Persistency/Common/HLTPathStatus.cc
    Persistency/Common/PrincipalBase.cc
    Persistency/Common/PtrVectorBase.cc
    Persistency/Common/RNGsnapshot.cc
    Persistency/Common/RefCore.cc
    Persistency/Common/TriggerResults.cc
    Persistency/Common/detail/aggregate.cc
    Persistency/Common/detail/maybeCastObj.cc
    Persistency/Common/detail/throwPartnerException.cc
    Persistency/Common/traits.cc
    Persistency/Provenance/BranchChildren.cc
    Persistency/Provenance/BranchDescription.cc
    Persistency/Provenance/BranchKey.cc
    Persistency/Provenance/BranchType.cc
    Persistency/Provenance/Compatibility/BranchID.cc
    Persistency/Provenance/Compatibility/History.cc
    Persistency/Provenance/EventAuxiliary.cc
    Persistency/Provenance/EventID.cc
    Persistency/Provenance/EventRange.cc
    Persistency/Provenance/FileFormatVersion.cc
    Persistency/Provenance/FileIndex.cc
    Persistency/Provenance/Hash.cc
    Persistency/Provenance/ParameterSetBlob.cc
    Persistency/Provenance/Parentage.cc
    Persistency/Provenance/ProcessConfiguration.cc
    Persistency/Provenance/ProcessHistory.cc
    Persistency/Provenance/ProductID.cc
    Persistency/Provenance/ProductProvenance.cc
    Persistency/Provenance/ProductTables.cc
    Persistency/Provenance/RangeSet.cc
    Persistency/Provenance/ResultsAuxiliary.cc
    Persistency/Provenance/RunAuxiliary.cc
    Persistency/Provenance/RunID.cc
    Persistency/Provenance/SubRunAuxiliary.cc
    Persistency/Provenance/SubRunID.cc
    Persistency/Provenance/Timestamp.cc
    Persistency/Provenance/TypeLabel.cc
    Persistency/Provenance/canonicalProductName.cc
    Persistency/Provenance/detail/createProductLookups.cc
    Persistency/Provenance/detail/createViewLookups.cc
    Persistency/Provenance/rootNames.cc
    Utilities/DebugMacros.cc
    Utilities/EventIDMatcher.cc
    Utilities/Exception.cc
    Utilities/FriendlyName.cc
    Utilities/InputTag.cc
    Utilities/TypeID.cc
    Utilities/WrappedClassName.cc
    Utilities/uniform_type_name.cc
  LIBRARIES
  PUBLIC
    fhiclcpp::fhiclcpp
    cetlib::container_algorithms
    cetlib::cetlib
    hep_concurrency::hep_concurrency
    cetlib_except::cetlib_except
  PRIVATE
    messagefacility::MF_MessageLogger
    CLHEP::Matrix
    CLHEP::Vector
    Boost::date_time
    ${CMAKE_DL_LIBS}
    $<$<PLATFORM_ID:Darwin>:c++abi>
)

add_subdirectory(Persistency/Common)
add_subdirectory(Version)

# Testing
add_subdirectory(test)

set(install_subdirs
  Persistency/Provenance
  Persistency/Provenance/Compatibility
  Persistency/Provenance/Compatibility/detail
  Persistency/Provenance/detail
  Utilities
)
install_headers(SUBDIRS ${install_subdirs})
install_source(SUBDIRS ${install_subdirs})
