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

set(YF_DIR_INCLUDE_LIBFS "${PROJECT_SOURCE_DIR}/include/libfs")
include_directories(${YF_DIR_INCLUDE_LIBFS})

set(YF_DIR_LIBFS "${PROJECT_SOURCE_DIR}/libfs")
include_directories(${YF_DIR_LIBFS})

include_directories(${PROJECT_BINARY_DIR})

aux_source_directory(. YF_DIR_TEST)

#
# Add the executable
#
add_executable(fs-test
  ${YF_DIR_TEST}
  )

#
# Linke the library
#
target_link_libraries(fs-test dl)

#
# Add the install targets
#
install(
  TARGETS fs-test
  RUNTIME DESTINATION ${YF_BINDIR}
  )
