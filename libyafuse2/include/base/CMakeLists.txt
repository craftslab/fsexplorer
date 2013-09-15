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
