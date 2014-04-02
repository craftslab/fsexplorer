/**
 * file.c - file of FAT.
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

#include "config.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#ifdef DEBUG
#define DEBUG_LIBFAT_FILE
#endif

#include "include/base/debug.h"
#include "include/base/types.h"
#include "include/libio/io.h"
#include "include/libfat/libfat.h"

/*
 * Macro Definition
 */

/*
 * Type Definition
 */

/*
 * Global Variable Definition
 */

/*
 * Function Declaration
 */

/*
 * Function Definition
 */
int32_t fat_fill_file(const struct fat_super_block *sb, int32_t cluster, int64_t size, uint8_t *buf)
{
  int32_t sector = 0;
  int64_t offset = 0;
  int32_t ret = 0;

  ret = fat_fill_clus2sec(sb, cluster, &sector);
  if (ret != 0) {
    return -1;
  }

  offset = sector * (int32_t)GET_UNALIGNED_LE16(sb->bs.sector_size);
  ret = io_seek(offset);
  if (ret != 0) {
    return -1;
  }

  ret = io_read(buf, size);
  if (ret != 0) {
    return -1;
  }

  return 0;
}
