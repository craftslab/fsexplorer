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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>

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
static FILE *io_fd = NULL;

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

  if (io_fd) {
    error("close io first.");
    return -1;
  }

  io_fd = fopen(fs_name, "rb+");
  if (!io_fd) {
    return -1;
  }

  return 0;
}

/*
 * Close IO
 */
void io_close(void)
{
  if (!io_fd) {
    return;
  }

  (void)fclose(io_fd);

  io_fd = NULL;
}

/*
 * Seek IO of file
 */
int32_t io_seek(int64_t offset)
{
  int32_t ret = 0;

  if (offset < 0) {
    return -1;
  }    

  if (!io_fd) {
    error("invalid args!");
    return -1;
  }    

  ret = fseek(io_fd, (long)offset, SEEK_SET);
  if (ret < 0) {
    return -1;
  }

  return 0;
}

/*
 * Read IO of file
 */
int32_t io_read(uint8_t *data, int64_t len)
{
  size_t ret = 0;

  if (data == NULL || len <= 0) {
    return -1;
  }    

  if (!io_fd) {
    error("invalid args!");
    return -1;
  }    

  ret = fread((void *)data, 1, (size_t)len, io_fd);
  if (ret == 0) {
    return -1;
  }

  return 0;
}

/*
 * Write IO of file
 */
int32_t io_write(uint8_t *data, int64_t len)
{
  size_t ret = 0;

  if (data == NULL || len <= 0) {
    return -1;
  }

  if (!io_fd) {
    error("invalid args!");
    return -1;
  }

  ret = fwrite((const void *)data, 1, (size_t)len, io_fd);
  if (ret == 0) {
    return -1;
  }

  fflush(io_fd);

  return 0;
}
