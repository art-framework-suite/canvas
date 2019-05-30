#
#  canvas/CMakeLists.txt
#  -------------------------------------------------------------------
#
#  CMake build file for library canvas 
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

cmake_minimum_required (VERSION 3.11) # use cmake 3.11 or later

project(canvas)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# cetbuildtools contains our cmake modules
find_package(cetbuildtools 7.13.01 REQUIRED)

include(CetCMakeEnv)
cet_cmake_env()

cet_set_compiler_flags(DIAGS VIGILANT
  WERROR
  NO_UNDEFINED
  EXTRA_FLAGS -pedantic
  EXTRA_CXX_FLAGS -Wnon-virtual-dtor -Wdelete-non-virtual-dtor
  )
cet_have_qual(sse2 SSE2)
if ( SSE2 )
  cet_add_compiler_flags(CXX -msse2 -ftree-vectorizer-verbose=2)
endif()

if($ENV{CET_SANITIZE_ADDRESS})
  if(NOT "$ENV{GCC_FQ_DIR}" STREQUAL "")
    find_file(ASAN_LIB libasan.so NO_DEFAULT_PATH PATHS $ENV{GCC_FQ_DIR}/lib64)
  endif()
endif()

if($ENV{CET_SANITIZE_THREAD})
  if(NOT "$ENV{GCC_FQ_DIR}" STREQUAL "")
    find_file(TSAN_LIB libtsan.so NO_DEFAULT_PATH PATHS $ENV{GCC_FQ_DIR}/lib64)
  endif()
endif()

if ($ENV{CET_SANITIZE_ADDRESS})
cet_add_compiler_flags(CXX -fsanitize=address)
cet_add_compiler_flags(C -fsanitize=address)
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fsanitize=address")
set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS_DEBUG} -fsanitize=address")
set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -fsanitize=address")
endif()

if ($ENV{CET_SANITIZE_THREAD})
cet_add_compiler_flags(CXX -fsanitize=thread -D__SANITIZE_THREAD__)
cet_add_compiler_flags(C -fsanitize=thread -D__SANITIZE_THREAD__)
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fsanitize=thread -D__SANITIZE_THREAD__")
set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS_DEBUG} -fsanitize=thread -D__SANITIZE_THREAD__")
set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -fsanitize=thread -D__SANITIZE_THREAD__")
endif()

cet_report_compiler_flags()

# these are minimum required versions, not the actual product versions
find_ups_product(cppunit)
find_ups_product(clhep)
find_ups_product(cetlib_except)
find_ups_product(cetlib v2_01_00)
#find_ups_product(python v3_7_2)
find_ups_product(fhiclcpp)
find_ups_product(messagefacility)
find_ups_product(range)
find_ups_boost(BOOST_TARGETS)
find_ups_product(cetbuildtools)

# ----------------------------------------------------------------------
# Python stuff

#cet_find_library(PYTHON NAMES python2.7 PATHS ENV PYTHON_LIB NO_DEFAULT_PATH)
cet_find_library(PYTHON NAMES python3.7 PATHS ENV PYTHON_LIB NO_DEFAULT_PATH)

# define CLHEP
cet_find_library(CLHEP  NAMES CLHEP   PATHS ENV CLHEP_LIB_DIR NO_DEFAULT_PATH)

# define cppunit
cet_find_library(CPPUNIT NAMES cppunit   PATHS ENV CPPUNIT_LIB   NO_DEFAULT_PATH)

# ----------------------------------------------------------------------
# Describe source tree layout:

add_subdirectory(canvas) # source
add_subdirectory(tools) # tools
add_subdirectory(ups) # ups - table and config files

# ----------------------------------------------------------------------
# Documentation generation:
#find_package(Sphinx REQUIRED)
if(BUILD_DOCS)
    add_subdirectory(docs)
else()
    message("Not going to build documentation because variable BUILD_DOCS was not set..")
endif()

# ----------------------------------------------------------------------
# Packaging utility
include(UseCPack)
