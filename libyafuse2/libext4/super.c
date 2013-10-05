/**
 * super.c - Superblock of Ext4.
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
#include <math.h>

#ifdef DEBUG
// Add code here
#endif

#include "include/base/debug.h"
#include "include/base/types.h"
#include "include/libio/io.h"
#include "include/libext4/libext4.h"

/*
 * Macro Definition
 */
#define EXT4_GROUP_0_PAD_SZ  (1024)

#define EXT4_SUPER_MAGIC  (0xEF53)

/*
 * Type Definition
 */

/*
 * Global Variable Definition
 */

/*
 * Function Declaration
 */
#if 0 //suppress compiling error of -Werror=unused-function
static int32_t ext4_is_power_of(int32_t a, int32_t b);
static int32_t ext4_bg_has_sb(const struct ext4_super_block *sb, int32_t bg_idx);
#endif

/*
 * Function Definition
 */
#if 0 //suppress compiling error of -Werror=unused-function
static int32_t ext4_is_power_of(int32_t a, int32_t b)
{
  while (a > b) {
    if (a % b) {
      return 0;
    }
    a /= b;
  }

  return (a == b) ? 1 : 0;
}

static int32_t ext4_bg_has_sb(const struct ext4_super_block *sb, int32_t bg_idx)
{
  if (!(sb->s_feature_ro_compat & EXT4_FEATURE_RO_COMPAT_SPARSE_SUPER)) {
    return 1;
  }

  if (bg_idx == 0 || bg_idx == 1) {
    return 1;
  }

  if (ext4_is_power_of(bg_idx, 3) || ext4_is_power_of(bg_idx, 5) || ext4_is_power_of(bg_idx, 7)) {
    return 1;
  }

  return 0;
}
#endif

int32_t ext4_raw_super(struct ext4_super_block *sb)
{
  int32_t offset = 0;
  size_t sb_sz = 0;
  int32_t ret = 0;

  offset = EXT4_GROUP_0_PAD_SZ;

  ret = io_seek((long)offset);
  if (ret != 0) {
    return -1;
  }

  /*
   * Fill in Ext4 superblock
   * default size of superblock is 1024 bytes
   */
  sb_sz = sizeof(struct ext4_super_block);
  ret = io_read((uint8_t *)sb, sb_sz);
  if (ret != 0) {
    memset((void *)sb, 0, sb_sz);
    return -1;
  }

  if (sb->s_magic != EXT4_SUPER_MAGIC) {
    return -1;
  }

  return 0;
}
