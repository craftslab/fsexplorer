#
# CMake file for simg2img
#
# From the off-tree build directory, invoke:
# $ cmake <PATH_TO_SIMG2IMG_ROOT>
#

#
# Set environment variables
#
set(SIMG2IMG_SRC_ROOT "${SIMG2IMG_SRC_DIR}/src")

include_directories(${SIMG2IMG_SRC_ROOT})

set(SIMG2IMG_SRC_BINFILES_SIMG2IMG
  "${SIMG2IMG_SRC_ROOT}/simg2img.c"
  "${SIMG2IMG_SRC_ROOT}/sparse_crc32.c"
  )

set(SIMG2IMG_SRC_BINNAME_SIMG2IMG "simg2img")

#
# Build target library
#
add_executable(${SIMG2IMG_SRC_BINNAME_SIMG2IMG}
  ${SIMG2IMG_SRC_BINFILES_SIMG2IMG}
  )

#
# Install the target
#
install(
  TARGETS ${SIMG2IMG_SRC_BINNAME_SIMG2IMG}
  RUNTIME DESTINATION ${SIMG2IMG_PREFIX_BIN_DIR}
  )
