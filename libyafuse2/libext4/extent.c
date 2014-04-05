/**
 * extent.c - extent of Ext4.
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
#define DEBUG_LIBEXT4_EXTENT
#endif

#include "include/base/debug.h"
#include "include/base/types.h"
#include "include/libio/io.h"
#include "include/libext4/libext4.h"

/*
 * Macro Definition
 */

/*
 * Type Definition
 */

/*
 * Global Variable Definition
 */
#ifdef DEBUG_LIBEXT4_EXTENT
static char buf[0x100];
#endif

/*
 * Function Declaration
 */

/*
 * Function Definition
 */
int32_t ext4_ext_depth(struct inode *inode, uint16_t *depth)
{
  struct ext4_extent_header eh;
  int32_t ret;

  ret = ext4_inode_hdr(inode, &eh);
  if (ret != 0) {
    return ret;
  }

  *depth = eh.eh_depth;

  return 0;
}

int32_t ext4_ext_find_extent(struct inode *inode, uint16_t depth, struct ext4_ext_path *path)
{
  struct ext4_extent_header eh;
  uint8_t *ee = NULL;
  int32_t ret;

  /*
   * In type of 'ext4_ext_path',
   * 'p_depth' > 1 is NOT supported yet, and
   * 'p_idx' is NOT supported yet
   */
  if (depth > 1) {
    return -1;
  }

  ret = ext4_inode_hdr(inode, &eh);
  if (ret != 0) {
    return ret;
  }
  memcpy((void *)path[0].p_hdr, (const void *)&eh, sizeof(struct ext4_extent_header));

  path[0].p_depth = (__u16)0;
  path[0].p_idx = NULL;

  ee = (uint8_t *)((uint8_t *)(inode->i_block) + sizeof(struct ext4_extent_header));
  memcpy((void *)path[0].p_ext, (const void *)ee, sizeof(struct ext4_extent));

#ifdef DEBUG_LIBEXT4_EXTENT
  memset((void *)buf, 0, sizeof(buf));
  ext4_show_extent_header(path[0].p_hdr, buf, sizeof(buf));
  fprintf(stdout, "%s", buf);

  memset((void *)buf, 0, sizeof(buf));
  ext4_show_extent(path[0].p_ext, buf, sizeof(buf));
  fprintf(stdout, "%s", buf);
#endif

  return 0;
}
