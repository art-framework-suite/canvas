# ======================================================================
#  art main build file
#
#  cd .../path/to/build/directory
#  source .../path/to/art/ups/setup_for_development
#  cmake [-DCMAKE_INSTALL_PREFIX=/install/path]
#        -DCMAKE_BUILD_TYPE=$CETPKG_TYPE
#        $CETPKG_SOURCE
#  make
#  make test
#  make install
#  make package (builds distribution tarfile)
# ======================================================================


cmake_minimum_required( VERSION 3.2 )

project(canvas VERSION 2.00.02 )

# cetmodules contains our cmake modules
find_package(cetmodules REQUIRED)

# module path
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Modules)

include(CetCMakeEnv)
cet_cmake_env()

cet_set_compiler_flags(DIAGS VIGILANT
  WERROR
  NO_UNDEFINED
  EXTRA_FLAGS -pedantic
  EXTRA_CXX_FLAGS -Wnon-virtual-dtor -Wdelete-non-virtual-dtor
  )
# no qualifiers ....
##cet_have_qual(sse2 SSE2)
##if ( SSE2 )
##  cet_add_compiler_flags(CXX -msse2 -ftree-vectorizer-verbose=2)
##endif()

cet_report_compiler_flags()

#find_ups_product(cppunit)
#find_ups_product(clhep)
set(CLHEP_DIR "${CLHEP_FQ_DIR}/lib/CLHEP-2.3.4.4")
find_package( CLHEP )
find_ups_product(messagefacility)
find_ups_product(tbb)
find_ups_root()
find_ups_boost()


# define CLHEP
cet_find_library( CLHEP  NAMES CLHEP   PATHS ENV CLHEP_LIB_DIR NO_DEFAULT_PATH )
include_directories ( $ENV{CLHEP_INC} )

# define cppunit
cet_find_library( CPPUNIT NAMES cppunit   PATHS ENV CPPUNIT_LIB   NO_DEFAULT_PATH )

# Python
cet_find_library(PYTHON NAMES python2.7 PATHS ENV PYTHON_LIB NO_DEFAULT_PATH)

# SQLite
cet_find_library(SQLITE3 NAMES sqlite3_ups PATHS ENV SQLITE_LIB NO_DEFAULT_PATH)

# art_dictionary
include(ArtDictionary)

# source
add_subdirectory (canvas)
add_subdirectory (tools)

# ups - table and config files
add_subdirectory(ups)

# CMake modules
add_subdirectory(Modules)

# packaging utility
#include(UseCPack)
