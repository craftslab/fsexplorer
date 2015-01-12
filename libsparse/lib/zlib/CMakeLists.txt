#
# CMake file for libsparse
#
# From the off-tree build directory, invoke:
# $ cmake <PATH_TO_HELADO_ROOT>
#

#
# Set environment variables
#
set(SPARSE_LIB_ZLIB_SRC_DIR "${SPARSE_LIB_ZLIB_ROOT}/src")
include_directories(${SPARSE_LIB_ZLIB_SRC_DIR})

set(SPARSE_LIB_ZLIB_SRC_FILES
  "${SPARSE_LIB_ZLIB_SRC_DIR}/adler32"
  "${SPARSE_LIB_ZLIB_SRC_DIR}/compress.c"
  "${SPARSE_LIB_ZLIB_SRC_DIR}/crc32.c"
  "${SPARSE_LIB_ZLIB_SRC_DIR}/deflate.c"
  "${SPARSE_LIB_ZLIB_SRC_DIR}/gzclose.c"
  "${SPARSE_LIB_ZLIB_SRC_DIR}/gzlib.c"
  "${SPARSE_LIB_ZLIB_SRC_DIR}/gzread.c"
  "${SPARSE_LIB_ZLIB_SRC_DIR}/gzwrite.c"
  "${SPARSE_LIB_ZLIB_SRC_DIR}/infback.c"
  "${SPARSE_LIB_ZLIB_SRC_DIR}/inflate.c"
  "${SPARSE_LIB_ZLIB_SRC_DIR}/inftrees.c"
  "${SPARSE_LIB_ZLIB_SRC_DIR}/inffast.c"
  "${SPARSE_LIB_ZLIB_SRC_DIR}/trees.c"
  "${SPARSE_LIB_ZLIB_SRC_DIR}/uncompr.c"
  "${SPARSE_LIB_ZLIB_SRC_DIR}/zutil.c"
  )

#
# Build target library
#
add_library(${SPARSE_LIB_ZLIB_LIBNAME}
  STATIC
  ${SPARSE_LIB_ZLIB_SRC_FILES}
  )
