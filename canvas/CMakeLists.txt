include(CetMake)

# cmake will automatically order the library builds according to declared dependencies
add_subdirectory(Version)
add_subdirectory(Utilities)
add_subdirectory(Persistency)


cet_make(LIBRARY_NAME canvas ### MIGRATE-ACTION-RECOMMENDED: use cet_make_library(), build_dictionary(), basic_plugin() with explicit source lists
  SUBDIRS Version ${CMAKE_CURRENT_BINARY_DIR}/Version
  Utilities
  Utilities/detail
  Persistency/Provenance
  Persistency/Provenance/Compatibility
  Persistency/Provenance/detail
  Persistency/Common
  Persistency/Common/detail
  LIBRARIES
  messagefacility::MF_MessageLogger
  $<$<PLATFORM_ID:Darwin>:c++abi>
  fhiclcpp::fhiclcpp
  cetlib::cetlib
  Boost::date_time
  ${CLHEP}
  ${CMAKE_DL_LIBS}
  NO_DICTIONARY)

# Testing
add_subdirectory(test)
