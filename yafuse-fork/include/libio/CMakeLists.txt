#
#  CMake file for Yafuse
#
#  From the off-tree build directory, invoke:
#  $ cmake <PATH_TO_HELADO_ROOT>
#

#
# Add source directory
#
set(YF_DIR_INCLUDE_BASE "${PROJECT_SOURCE_DIR}/include/base")
include_directories(${YF_DIR_INCLUDE_BASE})

include_directories(${PROJECT_BINARY_DIR})

aux_source_directory(. YF_DIR_INCLUDE_LIBIO)

#
# Generate the library
#
#add_library(libio STATIC ${YF_DIR_INCLUDE_LIBIO})

#
# Link the library
#
#target_link_libraries(libio m)
