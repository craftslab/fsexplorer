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

set(YF_DIR_INCLUDE_LIBEXT4 "${PROJECT_SOURCE_DIR}/include/libext4")
include_directories(${YF_DIR_INCLUDE_LIBEXT4})

set(YF_DIR_INCLUDE_LIBFAT "${PROJECT_SOURCE_DIR}/include/libfat")
include_directories(${YF_DIR_INCLUDE_LIBFAT})

set(YF_DIR_LIBFS "${PROJECT_SOURCE_DIR}/libfs")
include_directories(${YF_DIR_LIBFS})

set(YF_DIR_LIBEXT4 "${PROJECT_SOURCE_DIR}/libext4")
include_directories(${YF_DIR_LIBEXT4})

set(YF_DIR_LIBFAT "${PROJECT_SOURCE_DIR}/libfat")
include_directories(${YF_DIR_LIBFAT})

include_directories(${PROJECT_BINARY_DIR})

aux_source_directory(. YF_DIR_SRC)

#
# Add the executable
#
add_executable(yafuse
  ${YF_DIR_SRC}
  )

#
# Add the dependency
#
add_dependencies(yafuse io)
add_dependencies(yafuse fs)
add_dependencies(yafuse ext4)
add_dependencies(yafuse fat)

#
# Link the library
#
target_link_libraries(yafuse ncurses)
target_link_libraries(yafuse readline)
target_link_libraries(yafuse io)
target_link_libraries(yafuse fs)
target_link_libraries(yafuse ext4)
target_link_libraries(yafuse fat)

#
# Add the install targets
#
install(
  TARGETS yafuse
  RUNTIME DESTINATION ${YF_BINDIR}
  LIBRARY DESTINATION ${YF_LIBDIR}
  )
