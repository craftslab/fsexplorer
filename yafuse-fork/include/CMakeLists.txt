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
include_directories("${YF_DIR_INCLUDE_BASE}")

set(YF_DIR_INCLUDE_LIBIO "${PROJECT_SOURCE_DIR}/include/libio")
include_directories("${YF_DIR_INCLUDE_LIBIO}")

set(YF_DIR_INCLUDE_LIBFS "${PROJECT_SOURCE_DIR}/include/libfs")
include_directories("${YF_DIR_INCLUDE_LIBFS}")

set(YF_DIR_INCLUDE_LIBEXT4 "${PROJECT_SOURCE_DIR}/include/libext4")
include_directories("${YF_DIR_INCLUDE_LIBEXT4}")

set(YF_DIR_INCLUDE_LIBFAT "${PROJECT_SOURCE_DIR}/include/libfat")
include_directories("${YF_DIR_INCLUDE_LIBFAT}")

aux_source_directory(. YF_DIR_INCLUDE)

add_subdirectory(base)
add_subdirectory(libio)
add_subdirectory(libfs)
add_subdirectory(libext4)
add_subdirectory(libfat)

