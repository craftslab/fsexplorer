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
include_directories("${YF_DIR}")

set(YF_DIR_INCLUDE_BASE "${PROJECT_SOURCE_DIR}/include/base")
include_directories("${YF_DIR_INCLUDE_BASE}")

set(YF_DIR_INCLUDE_LIBIO "${PROJECT_SOURCE_DIR}/include/libio")
include_directories("${YF_DIR_INCLUDE_LIBIO}")

set(YF_DIR_INCLUDE_LIBFAT "${PROJECT_SOURCE_DIR}/include/libfat")
include_directories("${YF_DIR_INCLUDE_LIBFAT}")

include_directories(${PROJECT_BINARY_DIR})

aux_source_directory(. YF_DIR_LIBFAT)

#
# Generate the library
#
add_library(fat
  SHARED
  ${YF_DIR_LIBFAT}
  )

#
# Linke the library
#
target_link_libraries(fat m)
target_link_libraries(fat io)

#
# Add the install targets
#
install(
  TARGETS fat
  LIBRARY DESTINATION ${YF_LIBDIR}
  )
