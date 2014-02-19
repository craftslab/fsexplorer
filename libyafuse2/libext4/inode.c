/**
 * inode.c - inode of Ext4.
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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#ifdef DEBUG
#define DEBUG_LIBEXT4_INODE
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
static inline int32_t ext4_valid_inum(struct super_block *sb, uint64_t ino);
static inline uint64_t ext4_inode_table(struct super_block *sb, struct ext4_group_desc *gdp);

/*
 * Function Definition
 */
static inline int32_t ext4_valid_inum(struct super_block *sb, uint64_t ino)
{
  struct ext4_sb_info *info = (struct ext4_sb_info *)(sb->s_fs_info);
  struct ext4_super_block *es = info->s_es;

  return (ino == EXT4_ROOT_INO
          || ino == EXT4_JOURNAL_INO
          || ino == EXT4_RESIZE_INO
          || (ino >= es->s_first_ino && ino <= es->s_inodes_count));
}

static inline uint64_t ext4_inode_table(struct super_block *sb, struct ext4_group_desc *gdp)
{
  struct ext4_sb_info *info = (struct ext4_sb_info *)(sb->s_fs_info);
  struct ext4_super_block *es = info->s_es;

  /*
   * Use 'struct ext4_group_desc_min' instead of 'struct ext4_group_desc' here
   */
  return gdp->bg_inode_table_lo
    | (es->s_desc_size >= EXT4_MIN_DESC_SIZE_64BIT ? (uint64_t)(gdp->bg_inode_table_hi) << 32 : 0);
}

int32_t ext4_inode_hdr(struct inode *inode, struct ext4_extent_header *eh)
{
  memcpy((void *)eh, (const void *)inode->i_block, sizeof(struct ext4_extent_header));

  return 0;
}

int32_t ext4_raw_inode(struct super_block *sb, uint64_t ino, struct ext4_inode *inode)
{
  struct ext4_sb_info *info = (struct ext4_sb_info *)(sb->s_fs_info);
  struct ext4_super_block *es = info->s_es;
  struct ext4_group_desc *gdp = NULL;
  ext4_group_t bg;
  int32_t inodes_per_block, inode_offset;
  int64_t start, offset;
  int32_t ret;

  if (!ext4_valid_inum(sb, ino)) {
    return -1;
  }

  bg = (ext4_group_t)((ino - 1) / info->s_inodes_per_group);
  gdp = &info->s_group_desc[bg];

  inodes_per_block = (int32_t)info->s_inodes_per_block;
  inode_offset = (int32_t)((ino - 1) % info->s_inodes_per_group);

  start = (int64_t)((ext4_inode_table(sb, gdp) + (inode_offset / inodes_per_block)) * sb->s_blocksize);
  offset = (int64_t)((inode_offset % inodes_per_block) * es->s_inode_size);

  ret = io_seek(start + offset);
  if (ret != 0) {
    return -1;
  }

  ret = io_read((uint8_t *)inode, sizeof(struct ext4_inode));
  if (ret != 0) {
    return -1;
  }

#ifdef DEBUG_LIBEXT4_INODE
  ext4_show_inode_stat(es, ino, inode);
#endif

  return 0;
}
