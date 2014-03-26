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
add_library(${YF_LIB_LIBEXT4}
  STATIC 
  ${YF_DIR_LIBEXT4}
  )

#
# Add the dependency
#
add_dependencies(${YF_LIB_LIBEXT4} ${YF_LIB_LIBIO})

#
# Linke the library
#
if (CMAKE_COMPILER_IS_GNUCC)
  target_link_libraries(${YF_LIB_LIBEXT4} m)
  target_link_libraries(${YF_LIB_LIBEXT4} ${YF_LIB_LIBIO})
else (CMAKE_COMPILER_IS_GNUCC)
  target_link_libraries(${YF_LIB_LIBEXT4} ${YF_LIB_LIBIO})
endif (CMAKE_COMPILER_IS_GNUCC)

#
# Strip the library
#
#if (CMAKE_COMPILER_IS_GNUCC)
#  if (DEFINED CMAKE_STRIP)
#    execute_process(COMMAND ${CMAKE_STRIP} ${CMAKE_BINARY_DIR}/libext4/libext4.so)
#  endif (DEFINED CMAKE_STRIP)
#endif (CMAKE_COMPILER_IS_GNUCC)

#
# Add the install targets
#
#install(
#  TARGETS ${YF_LIB_LIBEXT4}
#  ARCHIVE DESTINATION ${YF_LIBDIR}
#  LIBRARY DESTINATION ${YF_LIBDIR}
#  COMPONENT library
#  )
