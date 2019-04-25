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


# use cmake 3.11 or later
cmake_minimum_required (VERSION 3.11...3.14 FATAL_ERROR)

project(canvas VERSION 3.07.03 LANGUAGES CXX)

# cetmodules contains our cmake modules
find_package(cetmodules REQUIRED)

include(CetCMakeEnv)
cet_cmake_env()

cet_set_compiler_flags(DIAGS VIGILANT
  WERROR
  NO_UNDEFINED
  EXTRA_FLAGS -pedantic
  EXTRA_CXX_FLAGS -Wnon-virtual-dtor -Wdelete-non-virtual-dtor
  )

cet_report_compiler_flags()

find_library(CPPUNIT NAMES cppunit REQUIRED)
find_package(messagefacility REQUIRED)
find_package(fhiclcpp REQUIRED)
find_package(cetlib REQUIRED)
find_package(cetlib_except REQUIRED)
find_package(hep_concurrency REQUIRED)
find_package(CLHEP REQUIRED COMPONENTS Vector Matrix)
find_package(range-v3 REQUIRED)
find_package(Boost REQUIRED)

# source
add_subdirectory(canvas)
add_subdirectory(tools)

# ups - table and config files
add_subdirectory(cmakeConfig)
