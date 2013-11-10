/**
 * io.c - IO interface for filesystem.
 *
 * Copyright (c) 2013-2014 angersax@gmail.com
 *
 * This program/include file is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program/include file is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program (in the main directory of the distribution
 * in the file COPYING); if not, write to the Free Software
 * Foundation,Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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
int32_t io_seek(long offset)
{
  int32_t ret = 0;

  if (offset < 0) {
    return -1;
  }    

  if (!io_fd) {
    error("invalid args!");
    return -1;
  }    

  ret = fseek(io_fd, offset, SEEK_SET);
  if (ret < 0) {
    return -1;
  }

  return 0;
}

/*
 * Read IO of file
 */
int32_t io_read(uint8_t *data, size_t len)
{
  size_t ret = 0;

  if (data == NULL || len <= 0) {
    return -1;
  }    

  if (!io_fd) {
    error("invalid args!");
    return -1;
  }    

  ret = fread((void *)data, 1, len, io_fd);
  if (ret == 0) {
    return -1;
  }

  return 0;
}

/*
 * Write IO of file
 */
int32_t io_write(uint8_t *data, size_t len)
{
  size_t ret = 0;

  if (data == NULL || len <= 0) {
    return -1;
  }

  if (!io_fd) {
    error("invalid args!");
    return -1;
  }

  ret = fwrite((const void *)data, 1, len, io_fd);
  if (ret == 0) {
    return -1;
  }

  fflush(io_fd);

  return 0;
}
