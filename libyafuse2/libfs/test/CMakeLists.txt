#
#  CMake file for libyafuse2
#
#  From the off-tree build directory, invoke:
#  $ cmake <PATH_TO_HELADO_ROOT>
#

#
# Add source directory
#
set(YF_BIN_LIBYAFUSE2_TEST "libyafuse2-test")

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
add_executable(${YF_BIN_LIBYAFUSE2_TEST}
  ${YF_DIR_TEST}
  )

#
# Linke the library
#
if (CMAKE_COMPILER_IS_GNUCC)
  target_link_libraries(${YF_BIN_LIBYAFUSE2_TEST} dl)
endif (CMAKE_COMPILER_IS_GNUCC)

#
# Add the install targets
#
install(
  TARGETS ${YF_BIN_LIBYAFUSE2_TEST}
  RUNTIME DESTINATION ${YF_BINDIR}
  )
