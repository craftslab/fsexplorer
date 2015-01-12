#
# CMake file for libsparse
#
# From the off-tree build directory, invoke:
# $ cmake <PATH_TO_HELADO_ROOT>
#

#
# Check cmake version
#
cmake_minimum_required(VERSION 2.6)

#
# Set project name
#
project(libsparse)
set(SPARSE_PROJECT_NAME "libsparse")

#
# Set version number
#
set(SPARSE_VERSION_MAJOR 1)
set(SPARSE_VERSION_MINOR 0)
set(SPARSE_VERSION_FULL "${SPARSE_VERSION_MAJOR}.${SPARSE_VERSION_MINOR}")

#
# Set environment variables
#
set(SPARSE_PREFIX ${CMAKE_INSTALL_PREFIX})
set(SPARSE_BIN_DIR "${SPARSE_PREFIX}/bin")
set(SPARSE_LIB_DIR "${SPARSE_PREFIX}/lib${LIB_SUFFIX}")
set(SPARSE_INC_DIR "${SPARSE_PREFIX}/include")

set(SPARSE_SRC_DIR "${PROJECT_SOURCE_DIR}")

#
# Set options for debug/release build
#
set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose build type.")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(DEBUG 1)
else (CMAKE_BUILD_TYPE STREQUAL "Release")
  set(RELEASE 1)
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

#
# Set options for 32-bit/64-bit target build
#
set(SPARSE_MACH_BIT 64 CACHE STRING "Choose target type.")

if (SPARSE_MACH_BIT STREQUAL "32")
  set(MACH "-m32")
else (SPARSE_MACH_BIT STREQUAL "64")
  set(MACH "-m64")
endif (SPARSE_MACH_BIT STREQUAL "32")

#
# Set compiling flags
#
if (CMAKE_COMPILER_IS_GNUCC)
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -O0 -Wall -Werror -fPIC -g2 ${MACH}")
  #set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} -O2 -Wall -Werror -Wpointer-arith -Wstrict-prototypes -Wstrict-overflow=3 -fPIC -g0 ${MACH}")
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} -O2 -Wall -Werror -Wpointer-arith -fPIC -g0 ${MACH}")
endif (CMAKE_COMPILER_IS_GNUCC)

#
# Configure header file to pass cmake settings to source code
#
#configure_file(
#  "${SPARSE_SRC_DIR}/src/config.h.in"
#  "${PROJECT_BINARY_DIR}/src/config.h"
#  )

#
# Set target name
#
if (CMAKE_COMPILER_IS_GNUCC)
  set(SPARSE_TARGET_NAME "sparse")
else (CMAKE_COMPILER_IS_GNUCC)
  set(SPARSE_TARGET_NAME "libsparse")
endif (CMAKE_COMPILER_IS_GNUCC)

#
# Include subdirectory
#
add_subdirectory(lib)
add_subdirectory(src)
