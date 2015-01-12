#
# CMake file for libps
#
# From the off-tree build directory, invoke:
# $ cmake <PATH_TO_HELADO_ROOT>
#

#
# Set environment variables
#
set(SPARSE_LIB_ZLIB_ROOT "${SPARSE_SRC_DIR}/lib/zlib")

if (CMAKE_COMPILER_IS_GNUCC)
  set(SPARSE_LIB_ZLIB_LIBNAME "z")
else (CMAKE_COMPILER_IS_GNUCC)
  set(SPARSE_LIB_ZLIB_LIBNAME "libz")
endif (CMAKE_COMPILER_IS_GNUCC)

include_directories(${SPARSE_LIB_ZLIB_ROOT})
