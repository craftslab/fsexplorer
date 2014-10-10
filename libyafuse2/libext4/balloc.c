/**
 * balloc.c - allocation of block group of Ext4.
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
#ifdef DEBUG_LIBEXT4_BALLOC
static char buf[EXT4_SHOW_STAT_GDP_SZ];
#endif

/*
 * Function Declaration
 */
static inline int32_t ext4_test_root(ext4_group_t a, uint32_t b);
static inline uint64_t ext4_group_first_block_no(struct super_block *sb, ext4_group_t bg);
static int32_t ext4_group_sparse(ext4_group_t bg);

/*
 * Function Definition
 */
static inline int32_t ext4_test_root(ext4_group_t a, uint32_t b)
{
  uint32_t num = b;

  while (a > (ext4_group_t)num) {
    num *= b;
  }

  return (int32_t)(num == a);
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

#if 0 // DISUSED here
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

  ret = io_read((uint8_t *)gdp, (int64_t)EXT4_DESC_SIZE(es));
  if (ret != 0) {
    return -1;
  }

#ifdef DEBUG_LIBEXT4_BALLOC
  memset((void *)buf, 0, sizeof(buf));
  ext4_show_stat_gdp(es, bg, gdp, buf, sizeof(buf));
  fprintf(stdout, "%s", buf);
#endif

  return 0;
}
#else
int32_t ext4_raw_group_desc(struct super_block *sb, uint32_t bg_cnt, struct ext4_group_desc *gdp)
{
  struct ext4_sb_info *info = (struct ext4_sb_info *)(sb->s_fs_info);
  struct ext4_super_block *es = info->s_es;
  int64_t has_super, offset;
  uint32_t i;
  int32_t ret;

  /*
   * Ignore the feature of EXT4_FEATURE_INCOMPAT_META_BG
   * (ext4_super_block: s_first_meta_bg)
   */
  // TODO

  for (i = 0; i < bg_cnt; ++i) {
    if (ext4_bg_has_super(sb, i)) {
      has_super = 1;

      offset = has_super + (int64_t)(ext4_group_first_block_no(sb, i));
      ret = io_seek((int64_t)(offset * sb->s_blocksize));
      if (ret != 0) {
        return -1;
      }

      ret = io_read((uint8_t *)gdp, (int64_t)(EXT4_DESC_SIZE(es) * bg_cnt));
      if (ret != 0) {
        return -1;
      }

      break;
    }
  }

#ifdef DEBUG_LIBEXT4_BALLOC
  for (i = 0; i < bg_cnt; ++i) {
    memset((void *)buf, 0, sizeof(buf));
    ext4_show_stat_gdp(es, i, &gdp[i], buf, sizeof(buf));
    fprintf(stdout, "%s", buf);
  }
#endif

  return 0;
}
#endif
