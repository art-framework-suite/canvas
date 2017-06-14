include(CetMake)

# cmake will automatically order the library builds according to declared dependencies
add_subdirectory (Version)
add_subdirectory (Utilities)
add_subdirectory (Persistency)

find_package(Threads)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)

cet_make ( LIBRARY_NAME canvas
  SUBDIRS Version ${CMAKE_CURRENT_BINARY_DIR}/Version
  Utilities
  Utilities/detail
  Persistency/Provenance
  Persistency/Provenance/Compatibility
  Persistency/Common
  Persistency/Common/detail
  LIBRARIES  MF_MessageLogger
  ${ROOT_CORE}
  ${TBB}
  MF_Utilities
  fhiclcpp
  cetlib
  cetlib_except
  ${CLHEP}
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_REGEX_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_THREAD_LIBRARY}
  ${CMAKE_THREAD_LIBS_INIT}
  ${CMAKE_DL_LIBS}
  NO_DICTIONARY
  )

# Testing
add_subdirectory(test)
