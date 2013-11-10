/**
 * dir.c - directory of Ext4.
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

/*
 * Function Declaration
 */
static int32_t ext4_find_dentry(struct super_block *sb, struct ext4_ext_path *path, struct ext4_dir_entry_2 *dentry);

/*
 * Function Definition
 */
static int32_t ext4_find_dentry(struct super_block *sb, struct ext4_ext_path *path, struct ext4_dir_entry_2 *dentry)
{
  struct ext4_extent *ext = path->p_ext;
  uint64_t offset;
  size_t len;
  int32_t ret;

  offset = (((uint64_t)ext->ee_start_hi << 32) | (uint64_t)ext->ee_start_lo) * sb->s_blocksize;

  ret = io_seek((long)offset);
  if (ret != 0) {
    return -1;
  }

  ret = io_read((uint8_t *)&dentry->inode, sizeof(dentry->inode));
  if (ret != 0) {
    return -1;
  }

  ret = io_read((uint8_t *)&dentry->rec_len, sizeof(dentry->rec_len));
  if (ret != 0) {
    return -1;
  }

  len = (size_t)(dentry->rec_len <= sizeof(struct ext4_dir_entry_2) ? dentry->rec_len : sizeof(struct ext4_dir_entry_2));

  ret = io_read((uint8_t *)dentry + sizeof(dentry->inode) + sizeof(dentry->rec_len), (size_t)(len - sizeof(dentry->inode) - sizeof(dentry->rec_len)));
  if (ret != 0) {
    return -1;
  }

  return 0;
}

int32_t ext4_raw_dentry(struct inode *inode, struct ext4_dir_entry_2 *dentry)
{
  struct super_block *sb = inode->i_sb;
  struct ext4_extent_header eh;
  struct ext4_extent_idx ei;
  struct ext4_extent ext;
  struct ext4_ext_path path;
  uint16_t depth;
  int32_t ret;

  /*
   * Hash tree directories is NOT supported yet
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

  ret = ext4_find_dentry(sb, &path, dentry);
  if (ret != 0) {
    return -1;
  }

#ifdef DEBUG_LIBEXT4_DIR
  ext4_show_dentry(dentry);
#endif

  return 0;
}
