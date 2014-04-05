/**
 * dir.c - directory of Ext4.
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
#define DEBUG_LIBEXT4_DIR
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
#ifdef DEBUG_LIBEXT4_DIR
static char buf[0x100];
#endif

/*
 * Function Declaration
 */
static int32_t ext4_find_dentry(struct super_block *sb, struct ext4_ext_path *path, uint64_t offset, struct ext4_dir_entry_2 *dentry);

/*
 * Function Definition
 */
static int32_t ext4_find_dentry(struct super_block *sb, struct ext4_ext_path *path, uint64_t offset, struct ext4_dir_entry_2 *dentry)
{
  struct ext4_extent *ext = path->p_ext;
  int64_t dent_offset;
  int64_t len;
  int32_t ret;

  dent_offset = (((uint64_t)ext->ee_start_hi << 32) | (uint64_t)ext->ee_start_lo) * sb->s_blocksize + offset;
  ret = io_seek(dent_offset);
  if (ret != 0) {
    return -1;
  }

  ret = io_read((uint8_t *)&dentry->inode, sizeof(dentry->inode));
  if (ret != 0) {
    return -1;
  }

  dent_offset += sizeof(dentry->inode);
  ret = io_seek(dent_offset);
  if (ret != 0) {
    return -1;
  }

  ret = io_read((uint8_t *)&dentry->rec_len, sizeof(dentry->rec_len));
  if (ret != 0) {
    return -1;
  }

  len = (int64_t)(dentry->rec_len <= sizeof(struct ext4_dir_entry_2) ? dentry->rec_len : sizeof(struct ext4_dir_entry_2));
  if (dentry->inode == EXT4_UNUSED_INO && len == 0) {
    return 0;
  }

  len -= (int64_t)(sizeof(dentry->inode) + sizeof(dentry->rec_len));
  if (len <= 0) {
    return -1;
  }

  dent_offset += sizeof(dentry->rec_len);
  ret = io_seek(dent_offset);
  if (ret != 0) {
    return -1;
  }

  ret = io_read((uint8_t *)dentry + sizeof(dentry->inode) + sizeof(dentry->rec_len), len);
  if (ret != 0) {
    return -1;
  }

  return 0;
}

int32_t ext4_raw_dentry_num(struct dentry *parent, uint32_t *childs_num)
{
  struct super_block *sb = parent->d_sb;
  struct inode *inode = parent->d_inode;
  struct ext4_extent_header eh;
  struct ext4_extent_idx ei;
  struct ext4_extent ext;
  struct ext4_ext_path path;
  uint16_t depth;
  struct ext4_dir_entry_2 dentry;
  uint64_t offset;
  uint32_t i;
  int32_t ret;

  /*
   * Support for linear directories only
   * and hash tree directories is NOT supported yet
   */
  if (is_dx(inode)) {
    return -1;
  }

  ret = ext4_ext_depth(inode, &depth);
  if (ret != 0) {
    return -1;
  }

  /*
   * In type of 'ext4_ext_path',
   * 'p_depth' > 1 is NOT supported yet, and
   * 'p_idx' is NOT supported yet
   */
  if (depth > 1) {
    return -1;
  }

  path.p_depth = 0;
  path.p_hdr = (struct ext4_extent_header *)&eh;
  path.p_idx = (struct ext4_extent_idx *)&ei;
  path.p_ext = (struct ext4_extent *)&ext;
  ret = ext4_ext_find_extent(inode, depth, &path);
  if (ret != 0) {
    return -1;
  }

  offset = 0;
  i = 0;

  while (1) {
    memset((void *)&dentry, 0, sizeof(struct ext4_dir_entry_2));
    ret = ext4_find_dentry(sb, &path, offset, &dentry);
    if (ret != 0) {
      break;
    }

    if (dentry.inode == EXT4_UNUSED_INO) {
      ret = 0;
      break;
    }

    ++i;

    offset += dentry.rec_len <= sizeof(struct ext4_dir_entry_2) ? dentry.rec_len : sizeof(struct ext4_dir_entry_2);
  }

  *childs_num = i;

  return ret;
}

int32_t ext4_raw_dentry(struct dentry *parent, struct ext4_dir_entry_2 *childs, uint32_t childs_num)
{
  struct super_block *sb = parent->d_sb;
  struct inode *inode = parent->d_inode;
  struct ext4_extent_header eh;
  struct ext4_extent_idx ei;
  struct ext4_extent ext;
  struct ext4_ext_path path;
  uint16_t depth;
  struct ext4_dir_entry_2 dentry;
  uint64_t offset;
  uint32_t i;
  int32_t ret;

  /*
   * Support for linear directories only
   * and hash tree directories is NOT supported yet
   */
  if (is_dx(inode)) {
    return -1;
  }

  ret = ext4_ext_depth(inode, &depth);
  if (ret != 0) {
    return -1;
  }

  /*
   * In type of 'ext4_ext_path',
   * 'p_depth' > 1 is NOT supported yet, and
   * 'p_idx' is NOT supported yet
   */
  if (depth > 1) {
    return -1;
  }

  path.p_depth = 0;
  path.p_hdr = (struct ext4_extent_header *)&eh;
  path.p_idx = (struct ext4_extent_idx *)&ei;
  path.p_ext = (struct ext4_extent *)&ext;
  ret = ext4_ext_find_extent(inode, depth, &path);
  if (ret != 0) {
    return -1;
  }

  for (i = 0, offset = 0; i < childs_num; ++i) {
    memset((void *)&dentry, 0, sizeof(struct ext4_dir_entry_2));
    ret = ext4_find_dentry(sb, &path, offset, &dentry);
    if (ret != 0) {
      break;
    }

    if (dentry.inode == EXT4_UNUSED_INO) {
      ret = 0;
      break;
    }

    childs[i] = dentry;

    offset += dentry.rec_len <= sizeof(struct ext4_dir_entry_2) ? dentry.rec_len : sizeof(struct ext4_dir_entry_2);

#ifdef DEBUG_LIBEXT4_DIR
    memset((void *)buf, 0, sizeof(buf));
    ext4_show_dentry(&childs[i], buf, sizeof(buf));
    fprintf(stdout, "%s", buf);
#endif
  }

  parent->d_childnum = childs_num;

  return ret;
}
