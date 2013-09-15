#
#  CMake file for Yafuse
#
#  From the off-tree build directory, invoke:
#  $ cmake <PATH_TO_HELADO_ROOT>
#

#
# Add source directory
#
set(YF_DIR "${PROJECT_SOURCE_DIR}")
include_directories(${YF_DIR})

set(YF_DIR_INCLUDE_BASE "${PROJECT_SOURCE_DIR}/include/base")
include_directories(${YF_DIR_INCLUDE_BASE})

set(YF_DIR_INCLUDE_LIBIO "${PROJECT_SOURCE_DIR}/include/libio")
include_directories(${YF_DIR_INCLUDE_LIBIO})

set(YF_DIR_INCLUDE_LIBFS "${PROJECT_SOURCE_DIR}/include/libfs")
include_directories(${YF_DIR_INCLUDE_LIBFS})

include_directories(${PROJECT_BINARY_DIR})

aux_source_directory(. YF_DIR_LIBFS)

add_subdirectory(test)

#
# Generate the library
#
add_library(fs
  SHARED
  ${YF_DIR_LIBFS}
  )

#
# Linke the library
#
target_link_libraries(fs dl)

#
# Add the install targets
#
install(
  TARGETS fs
  ARCHIVE DESTINATION ${YF_LIBDIR}
  LIBRARY DESTINATION ${YF_LIBDIR}
  COMPONENT library
  )
