/**
 * balloc.c - allocation of block group of Ext4.
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
// Add code here
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
static inline int32_t test_root(ext4_group_t a, int32_t b);
static int32_t ext4_group_sparse(ext4_group_t bg);

/*
 * Function Definition
 */
static inline int32_t test_root(ext4_group_t a, int32_t b)
{
  int32_t num = b;

  while (a > num) {
    num *= b;
  }

  return num == a;
}

static int32_t ext4_group_sparse(ext4_group_t bg)
{
  if (bg <= 1) {
    return 1;
  }

  if (!(bg & 1)) {
    return 0;
  }

  return (test_root(bg, 7) || test_root(bg, 5) || test_root(bg, 3));
}

int32_t ext4_bg_has_super(struct super_block *sb, ext4_group_t bg)
{
  if (EXT4_HAS_RO_COMPAT_FEATURE(sb, EXT4_FEATURE_RO_COMPAT_SPARSE_SUPER)
      && !ext4_group_sparse(bg)) {
    return 0;
  }

  return 1;
}

int32_t ext4_raw_group_desc(struct super_block *sb, ext4_group_t bg, struct ext4_group_desc *gdp)
{
  struct ext4_sb_info *info = (struct ext4_sb_info *)(sb->s_fs_info);
  ext4_group_t ngroups = info->s_groups_count;

  if (bg >= ngroups) {
    return -1;
  }

  // add code here

  return 0;
}
