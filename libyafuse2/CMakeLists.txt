#
#  Root CMake file for libyafuse2
#
#  From the off-tree build directory, invoke:
#  $ cmake <PATH_TO_HELADO_ROOT>
#

#
# Check version of cmake
#
cmake_minimum_required(VERSION 2.6)

#
# Set name of project
#
project(libyafuse2)
set(YF_PROJECT_NAME \"libyafuse2\")

#
# The version number
#
set(YF_VERSION_MAJOR 1)
set(YF_VERSION_MINOR 0)
set(YF_VERSION_FULL "${YF_VERSION_MAJOR}.${YF_VERSION_MINOR}")

#
# The environment variables
#
set(YF_PREFIX ${CMAKE_INSTALL_PREFIX})
set(YF_BINDIR "\${YF_PREFIX}/bin")
set(YF_LIBDIR "\${YF_PREFIX}/lib${LIB_SUFFIX}")
set(YF_INCLUDEDIR "\${YF_PREFXI}/include")

#
# Options for library supported
#
if (CMAKE_COMPILER_IS_GNUCC)
  set(YF_READLINE_DIR_INCLUDE "/opt/readline-6.2/include")
  include_directories(${YF_READLINE_DIR_INCLUDE})
  link_directories("/opt/readline-6.2/lib")

  set(YF_NCURSES_DIR_INCLUDE "/opt/ncurses-5.9/include")
  include_directories(${YF_NCURSES_DIR_INCLUDE})
  link_directories("/opt/ncurses-5.9/lib")
endif (CMAKE_COMPILER_IS_GNUCC)

#
# Options for version selection
#
set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build.")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(DEBUG 1)
else (CMAKE_BUILD_TYPE STREQUAL "Release")
  set(RELEASE 1)
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

#
# Options for selection of 32-bit/64-bit machine
#
set(YF_MACH_BIT 64 CACHE STRING "Choose the bit of machine.")

if (YF_MACH_BIT STREQUAL "32")
  set(MACH "-m32")
else (YF_MACH_BIT STREQUAL "64")
  set(MACH "-m64")
endif (YF_MACH_BIT STREQUAL "32")

#
# The compiling setting
#
if (CMAKE_COMPILER_IS_GNUCC)
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -O0 -Wall -Werror -fPIC -g2 ${MACH}")
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} -O2 -Wall -Werror -Wpointer-arith -Wstrict-prototypes -Wstrict-overflow=3 -fPIC -g0 ${MACH}")
endif (CMAKE_COMPILER_IS_GNUCC)

#
# Configure a header file to pass some of the cmake settings
# to the source code
#
configure_file(
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
  )

#
# Add source directory
#
set(YF_LIB_LIBIO "io")
set(YF_LIB_LIBEXT4 "ext4")
set(YF_LIB_LIBFAT "fat")

if (CMAKE_COMPILER_IS_GNUCC)
  set(YF_LIB_LIBYAFUSE2 "yafuse2")
else (CMAKE_COMPILER_IS_GNUCC)
  set(YF_LIB_LIBYAFUSE2 "libyafuse2")
endif (CMAKE_COMPILER_IS_GNUCC)

aux_source_directory(. YF_DIR)

add_subdirectory(include)
add_subdirectory(libio)
add_subdirectory(libext4)
add_subdirectory(libfat)
add_subdirectory(libfs)
