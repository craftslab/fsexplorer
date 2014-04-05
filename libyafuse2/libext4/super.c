/**
 * super.c - Superblock of Ext4.
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
#include <math.h>

#ifdef DEBUG
#define DEBUG_LIBEXT4_SUPER
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
#ifdef DEBUG_LIBEXT4_SUPER
static char buf[EXT4_SHOW_STAT_SB_SZ];
#endif

/*
 * Function Declaration
 */

/*
 * Function Definition
 */
int32_t ext4_fill_super_info(struct super_block *sb, struct ext4_super_block *es, struct ext4_sb_info *info)
{
  uint64_t block_size = sb->s_blocksize;
  uint64_t blocks_count = (uint64_t)es->s_blocks_count_hi << 32 | (uint64_t)es->s_blocks_count_lo;
  int32_t ret;

  info->s_desc_size = (__le64)es->s_desc_size;
  info->s_inodes_per_block = (__le64)(block_size / es->s_inode_size);
  info->s_blocks_per_group = (__le64)es->s_blocks_per_group;
  info->s_inodes_per_group = (__le64)es->s_inodes_per_group;
  info->s_itb_per_group = (__le64)(info->s_inodes_per_group / info->s_inodes_per_block);
  info->s_groups_count = (ext4_group_t)((blocks_count - es->s_first_data_block + es->s_blocks_per_group - 1) / es->s_blocks_per_group);
  info->s_desc_per_block = (__le64)(block_size / info->s_desc_size);
 
  info->s_es = (struct ext4_super_block *)malloc(sizeof(struct ext4_super_block));
  if (!info->s_es) {
    ret = -1;
    goto ext4_fill_super_info_fail;
  }
  memcpy((void *)info->s_es, (const void *)es, sizeof(struct ext4_super_block));

  info->s_group_desc = (struct ext4_group_desc *)malloc((size_t)(info->s_groups_count * info->s_desc_size));
  if (!info->s_group_desc) {
    ret = -1;
    goto ext4_fill_super_info_fail;
  }
  memset((void *)info->s_group_desc, 0, (size_t)(info->s_groups_count * info->s_desc_size));

#if 0 // DISUSED here
  for (i = 0; i < info->s_groups_count; ++i) {
    ret = ext4_raw_group_desc(sb, i, &info->s_group_desc[i]);
    if (ret != 0) {
      goto ext4_fill_super_info_fail;
    }
  }
#else
  ret = ext4_raw_group_desc(sb, info->s_groups_count, info->s_group_desc);
  if (ret != 0) {
    goto ext4_fill_super_info_fail;
  }
#endif

  return 0;

 ext4_fill_super_info_fail:

  if (info->s_group_desc) {
    free((void *)info->s_group_desc);
    info->s_group_desc = NULL;
  }

  if (info->s_es) {
    free((void *)info->s_es);
    info->s_es = NULL;
  }

  return ret;
}

int32_t ext4_raw_super(struct ext4_super_block *sb)
{
  int64_t offset = 0;
  int64_t sb_sz = 0;
  int32_t ret = 0;

  offset = EXT4_GROUP_0_PAD_SZ;
  ret = io_seek(offset);
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
    memset((void *)sb, 0, (size_t)sb_sz);
    return -1;
  }

  if (sb->s_magic != EXT4_SUPER_MAGIC) {
    return -1;
  }

#ifdef DEBUG_LIBEXT4_SUPER
  memset((void *)buf, 0, sizeof(buf));
  ext4_show_stat_sb(sb, buf, sizeof(buf));
  fprintf(stdout, "%s", buf);
#endif

  return 0;
}
