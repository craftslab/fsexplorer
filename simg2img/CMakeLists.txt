#
# CMake file for simg2img
#
# From the off-tree build directory, invoke:
# $ cmake <PATH_TO_SIMG2IMG_ROOT>
#

#
# Check cmake version
#
cmake_minimum_required(VERSION 2.6)

#
# Set project name
#
project(simg2img)
set(SIMG2IMG_PROJECT_NAME "simg2img")

#
# Set version number
#
set(SIMG2IMG_VERSION_MAJOR 1)
set(SIMG2IMG_VERSION_MINOR 0)
set(SIMG2IMG_VERSION_FULL "${SIMG2IMG_VERSION_MAJOR}.${SIMG2IMG_VERSION_MINOR}")

#
# Set environment variables
#
set(SIMG2IMG_PREFIX_ROOT ${CMAKE_INSTALL_PREFIX})
set(SIMG2IMG_PREFIX_BIN_DIR "${SIMG2IMG_PREFIX_ROOT}/bin")
set(SIMG2IMG_SRC_DIR "${PROJECT_SOURCE_DIR}")

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
set(SIMG2IMG_MACH_BIT 64 CACHE STRING "Choose target type.")

if (SIMG2IMG_MACH_BIT STREQUAL "32")
  set(MACH "-m32")
else (SIMG2IMG_MACH_BIT STREQUAL "64")
  set(MACH "-m64")
endif (SIMG2IMG_MACH_BIT STREQUAL "32")

#
# Set compiling flags
#
if (CMAKE_COMPILER_IS_GNUCC)
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -O0 -Wall -Werror -fPIC -g2 ${MACH}")
  #set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} -O2 -Wall -Werror -Wpointer-arith -Wstrict-prototypes -Wstrict-overflow=3 -fPIC -g0 ${MACH}")
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} -O2 -Wall -Werror -Wpointer-arith -fPIC -g0 ${MACH}")
endif (CMAKE_COMPILER_IS_GNUCC)

#
# Include subdirectory
#
add_subdirectory(src)
