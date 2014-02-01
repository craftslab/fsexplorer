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
#define DEBUG_LIBEXT4_BALLOC
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
static inline int32_t ext4_test_root(ext4_group_t a, int32_t b);
static inline uint64_t ext4_group_first_block_no(struct super_block *sb, ext4_group_t bg);
static int32_t ext4_group_sparse(ext4_group_t bg);

/*
 * Function Definition
 */
static inline int32_t ext4_test_root(ext4_group_t a, int32_t b)
{
  int32_t num = b;

  while (a > (ext4_group_t)num) {
    num *= b;
  }

  return num == a;
}

static inline uint64_t ext4_group_first_block_no(struct super_block *sb, ext4_group_t bg)
{
  struct ext4_sb_info *info = (struct ext4_sb_info *)(sb->s_fs_info);
  struct ext4_super_block *es = info->s_es;

  return (uint64_t)(bg * es->s_blocks_per_group + es->s_first_data_block);
}

static int32_t ext4_group_sparse(ext4_group_t bg)
{
  if (bg <= 1) {
    return 1;
  }

  if (!(bg & 1)) {
    return 0;
  }

  return (ext4_test_root(bg, 7) || ext4_test_root(bg, 5) || ext4_test_root(bg, 3));
}

int32_t ext4_bg_has_super(struct super_block *sb, ext4_group_t bg)
{
  struct ext4_sb_info *info = (struct ext4_sb_info *)(sb->s_fs_info);
  struct ext4_super_block *es = info->s_es;

  if (((es->s_feature_ro_compat & EXT4_FEATURE_RO_COMPAT_SPARSE_SUPER) != 0)
      && !ext4_group_sparse(bg)) {
    return 0;
  }

  return 1;
}

int32_t ext4_raw_group_desc(struct super_block *sb, ext4_group_t bg, struct ext4_group_desc *gdp)
{
  struct ext4_sb_info *info = (struct ext4_sb_info *)(sb->s_fs_info);
  struct ext4_super_block *es = info->s_es;
  int64_t has_super, offset;
  int32_t ret;

  /*
   * Ignore the feature of EXT4_FEATURE_INCOMPAT_META_BG
   * (ext4_super_block: s_first_meta_bg)
   */
  // TODO

  if (ext4_bg_has_super(sb, bg)) {
    has_super = 1;
  } else {
    has_super = 0;
  }

  offset = has_super + (int64_t)(ext4_group_first_block_no(sb, bg));
  ret = io_seek((int64_t)(offset * sb->s_blocksize));
  if (ret != 0) {
    return -1;
  }

  ret = io_read((uint8_t *)gdp, (int64_t)es->s_desc_size);
  if (ret != 0) {
    return -1;
  }

#ifdef DEBUG_LIBEXT4_BALLOC
  ext4_show_gdp_stat(es, bg, gdp);
#endif

  return 0;
}
