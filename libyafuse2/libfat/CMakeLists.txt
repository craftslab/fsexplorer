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
if (CMAKE_COMPILER_IS_GNUCC)
add_library(fat
  SHARED
  ${YF_DIR_LIBFAT}
  )
else (CMAKE_COMPILER_IS_GNUCC)
add_library(libfat
  SHARED
  ${YF_DIR_LIBFAT}
  )
endif (CMAKE_COMPILER_IS_GNUCC)

#
# Linke the library
#
if (CMAKE_COMPILER_IS_GNUCC)
  target_link_libraries(fat m)
  target_link_libraries(fat io)
else (CMAKE_COMPILER_IS_GNUCC)
  target_link_libraries(libfat io)
endif (CMAKE_COMPILER_IS_GNUCC)

#
# Strip the library
#
if (CMAKE_COMPILER_IS_GNUCC)
  if (DEFINED CMAKE_STRIP)
    execute_process(COMMAND ${CMAKE_STRIP} ${CMAKE_BINARY_DIR}/libfat/libfat.so)
  endif (DEFINED CMAKE_STRIP)
endif (CMAKE_COMPILER_IS_GNUCC)

#
# Add the install targets
#
if (CMAKE_COMPILER_IS_GNUCC)
install(
  TARGETS fat
  ARCHIVE DESTINATION ${YF_LIBDIR}
  LIBRARY DESTINATION ${YF_LIBDIR}
  COMPONENT library
  )
else (CMAKE_COMPILER_IS_GNUCC)
install(
  TARGETS libfat
  ARCHIVE DESTINATION ${YF_LIBDIR}
  LIBRARY DESTINATION ${YF_LIBDIR}
  COMPONENT library
  )
endif (CMAKE_COMPILER_IS_GNUCC)
