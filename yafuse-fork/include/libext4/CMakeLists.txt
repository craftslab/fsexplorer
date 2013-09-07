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

aux_source_directory(. YF_DIR_INCLUDE_LIBEXT4)

#
# Generate the library
#
#add_library(libext4 STATIC ${YF_DIR_INCLUDE_LIBEXT4})

#
# Link the library
#
#target_link_libraries(libext4 m)
