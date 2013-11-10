/**
 * extent.c - extent of Ext4.
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
  ext4_show_extent_header(path[0].p_hdr);
  ext4_show_extent(path[0].p_ext);
#endif

  return 0;
}
