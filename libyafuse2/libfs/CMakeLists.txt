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

set(YF_DIR_INCLUDE_LIBFS "${PROJECT_SOURCE_DIR}/include/libfs")
include_directories(${YF_DIR_INCLUDE_LIBFS})

include_directories(${PROJECT_BINARY_DIR})

aux_source_directory(. YF_DIR_LIBFS)

add_subdirectory(test)

#
# Generate the library
#
add_library(${YF_LIB_LIBYAFUSE2}
  SHARED
  ${YF_DIR_LIBFS}
  )

#
# Add the dependency
#
add_dependencies(${YF_LIB_LIBYAFUSE2} ${YF_LIB_LIBIO})
add_dependencies(${YF_LIB_LIBYAFUSE2} ${YF_LIB_LIBEXT4})
add_dependencies(${YF_LIB_LIBYAFUSE2} ${YF_LIB_LIBFAT})

#
# Linke the library
#
if (CMAKE_COMPILER_IS_GNUCC)
  target_link_libraries(${YF_LIB_LIBYAFUSE2} ${YF_LIB_LIBIO})
  target_link_libraries(${YF_LIB_LIBYAFUSE2} ${YF_LIB_LIBEXT4})
  target_link_libraries(${YF_LIB_LIBYAFUSE2} ${YF_LIB_LIBFAT})
  target_link_libraries(${YF_LIB_LIBYAFUSE2} dl)
else (CMAKE_COMPILER_IS_GNUCC)
  target_link_libraries(${YF_LIB_LIBYAFUSE2} ${YF_LIB_LIBIO})
  target_link_libraries(${YF_LIB_LIBYAFUSE2} ${YF_LIB_LIBEXT4})
  target_link_libraries(${YF_LIB_LIBYAFUSE2} ${YF_LIB_LIBFAT})
endif (CMAKE_COMPILER_IS_GNUCC)

#
# Strip the library
#
#if (CMAKE_COMPILER_IS_GNUCC)
#  if (DEFINED CMAKE_STRIP)
#    execute_process(COMMAND ${CMAKE_STRIP} ${CMAKE_BINARY_DIR}/libfs/libfs.so)
#  endif (DEFINED CMAKE_STRIP)
#endif (CMAKE_COMPILER_IS_GNUCC)

#
# Add the install targets
#
install(
  TARGETS ${YF_LIB_LIBYAFUSE2}
  ARCHIVE DESTINATION ${YF_LIBDIR}
  LIBRARY DESTINATION ${YF_LIBDIR}
  COMPONENT library
  )
