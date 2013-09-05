#
#  Root CMake file for Yafuse
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
project(yafuse)
set(YF_PROJECT_NAME \"Yafuse\")

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
else ()
  set(YF_READLINE_DIR_INCLUDE "/opt/readline-6.2/include")
endif (CMAKE_COMPILER_IS_GNUCC)
include_directories("${YF_READLINE_DIR_INCLUDE}")

#
# Options for version selection
#
option(YF_VER_RELEASE "Release Version" ON)
option(YF_VER_DEBUG "Debug Version" OFF)

#
# The compiling setting
#
if (CMAKE_COMPILER_IS_GNUCC)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")
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
aux_source_directory(. YF_DIR)

add_subdirectory(include)
add_subdirectory(libio)
add_subdirectory(libfs)
add_subdirectory(libext4)
add_subdirectory(libfat)
add_subdirectory(src)
