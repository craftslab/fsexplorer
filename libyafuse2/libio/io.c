/**
 * io.c - IO interface for filesystem.
 *
 * Copyright (c) 2013-2014 angersax@gmail.com
 *
 * This file is part of libyafuse2.
 *
 * libyafuse2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libyafuse2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libyafuse2.  If not, see <http://www.gnu.org/licenses/>.
 */

#define _LARGEFILE64_SOURCE

#include "config.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#ifdef CMAKE_COMPILER_IS_GNUCC
#include <unistd.h>
#endif /* CMAKE_COMPILER_IS_GNUCC */

#ifdef DEBUG
#define DEBUG_LIBIO_IO
#endif

#include "include/base/debug.h"
#include "include/libio/io.h"

/*
 * Macro Definition
 */

/*
 * Type Definition
 */

/*
 * Global Variable Definition
 */
static int io_fd = -1;

/*
 * Function Declaration
 */

/*
 * Function Definition
 */

/*
 * Open IO
 */
int32_t io_open(const char *fs_name)
{
  if (fs_name == NULL) {
    return -1;
  }

  if (io_fd != -1) {
    error("close io first.");
    return -1;
  }

  io_fd = open64(fs_name, O_RDONLY);
  if (io_fd == -1) {
    return -1;
  }

  return 0;
}

/*
 * Close IO
 */
void io_close(void)
{
  if (io_fd == -1) {
    return;
  }

  (void)close(io_fd);

  io_fd = -1;
}

/*
 * Seek IO of file
 */
int32_t io_seek(int64_t offset)
{
  off64_t ret = -1;

  if (offset < 0) {
    return -1;
  }

  if (io_fd == -1) {
    error("invalid args!");
    return -1;
  }

  ret = lseek64(io_fd, offset, SEEK_SET);
  if (ret == -1) {
    return -1;
  }

  return 0;
}

/*
 * Read IO of file
 */
int32_t io_read(uint8_t *data, int64_t len)
{
  ssize_t ret = -1;

  if (data == NULL || len <= 0) {
    return -1;
  }

  if (io_fd == -1) {
    error("invalid args!");
    return -1;
  }

  ret = read(io_fd, (void *)data, (size_t)len);
  if (ret == -1) {
    return -1;
  }

  return 0;
}

/*
 * Write IO of file
 */
int32_t io_write(uint8_t *data, int64_t len)
{
  ssize_t ret = -1;

  if (data == NULL || len <= 0) {
    return -1;
  }

  if (io_fd == -1) {
    error("invalid args!");
    return -1;
  }

  ret = write(io_fd, (const void *)data, (size_t)len);
  if (ret == -1) {
    return -1;
  }

  return 0;
}
