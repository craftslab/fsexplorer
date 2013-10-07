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

/*
 * Function Definition
 */
int32_t ext4_fill_super_info(struct super_block *sb, struct ext4_super_block *es, struct ext4_sb_info *info)
{
  uint32_t block_size = sb->s_blocksize;
  uint64_t blocks_count = (uint64_t)es->s_blocks_count_hi << 32 | (uint64_t)es->s_blocks_count_lo;
  ext4_group_t i;
  int32_t ret;

  info->s_desc_size = (__le64)es->s_desc_size;  
  info->s_inodes_per_block = (__le64)(block_size / es->s_inode_size);
  info->s_blocks_per_group = (__le64)es->s_blocks_per_group;
  info->s_inodes_per_group = (__le64)es->s_inodes_per_group;
  info->s_itb_per_group = (__le64)(info->s_inodes_per_group / info->s_inodes_per_block);
  info->s_groups_count = (ext4_group_t)((blocks_count - es->s_first_data_block + es->s_blocks_per_group - 1) / es->s_blocks_per_group);
  info->s_desc_per_block = (__le64)(block_size / info->s_desc_size);
  info->s_es = (struct ext4_super_block *)es;

  info->s_group_desc = (struct ext4_group_desc *)malloc(info->s_groups_count * sizeof(struct ext4_group_desc));
  if (!info->s_group_desc) {
    return -1;
  }

  for (i = 0; i < info->s_groups_count; ++i) {
    ret = ext4_raw_group_desc(sb, i, &info->s_group_desc[i]);
    if (ret != 0) {
      goto ext4_fill_super_info_fail;
    }
  }

  return 0;

 ext4_fill_super_info_fail:

  if (info->s_group_desc) {
    free(info->s_group_desc);
    info->s_group_desc = NULL;
  }

  return ret;
}

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
