#
#  CMake file for Yafuse
#
#  From the off-tree build directory, invoke:
#  $ cmake <PATH_TO_HELADO_ROOT>
#

#
# Add source directory
#
include_directories(${PROJECT_BINARY_DIR})

aux_source_directory(. YF_DIR_INCLUDE_BASE)

#
# Generate the library
#
#add_library(base STATIC ${YF_DIR_INCLUDE_BASE})

#
# Link the library
#
#target_link_libraries(base m)
