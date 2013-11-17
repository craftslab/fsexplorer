#
#  CMake file for libyafuse2
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

set(YF_DIR_INCLUDE_LIBEXT4 "${PROJECT_SOURCE_DIR}/include/libext4")
include_directories(${YF_DIR_INCLUDE_LIBEXT4})

include_directories(${PROJECT_BINARY_DIR})

aux_source_directory(. YF_DIR_LIBEXT4)

#
# Generate the library
#
add_library(ext4
  SHARED
  ${YF_DIR_LIBEXT4}
  )

#
# Linke the library
#
if (CMAKE_COMPILER_IS_GNUCC)
  target_link_libraries(ext4 m)
endif (CMAKE_COMPILER_IS_GNUCC)

target_link_libraries(ext4 io)

#
# Add the install targets
#
if (CMAKE_COMPILER_IS_GNUCC)
install(
  TARGETS ext4
  ARCHIVE DESTINATION ${YF_LIBDIR}
  LIBRARY DESTINATION ${YF_LIBDIR}
  COMPONENT library
  )
else (CMAKE_COMPILER_IS_GNUCC)
install(
  TARGETS libext4
  ARCHIVE DESTINATION ${YF_LIBDIR}
  LIBRARY DESTINATION ${YF_LIBDIR}
  COMPONENT library
  )
endif (CMAKE_COMPILER_IS_GNUCC)
