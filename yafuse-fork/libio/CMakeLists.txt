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

include_directories(${PROJECT_BINARY_DIR})

aux_source_directory(. YF_DIR_LIBIO)

#
# Generate the library
#
add_library(io
  SHARED
  ${YF_DIR_LIBIO}
  )

#
# Linke the library
#
#target_link_libraries(io m)

#
# Add the install targets
#
install(
  TARGETS io
  ARCHIVE DESTINATION ${YF_LIBDIR}
  LIBRARY DESTINATION ${YF_LIBDIR}
  COMPONENT library
  )
