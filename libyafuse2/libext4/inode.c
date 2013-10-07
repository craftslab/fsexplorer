/**
 * inode.c - inode of Ext4.
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
static inline int32_t ext4_valid_inum(struct ext4_super_block *es, uint64_t ino);

/*
 * Function Definition
 */
static inline int32_t ext4_valid_inum(struct ext4_super_block *es, uint64_t ino)
{
  return (ino == EXT4_ROOT_INO
          || ino == EXT4_JOURNAL_INO
          || ino == EXT4_RESIZE_INO
          || (ino >= es->s_first_ino && ino <= es->s_inodes_count));
}

int32_t ext4_raw_inode(struct super_block *sb, uint64_t ino, struct ext4_inode *inode)
{
  struct ext4_sb_info *info = (struct ext4_sb_info *)(sb->s_fs_info);
  struct ext4_super_block *es = info->s_es;

  if (!ext4_valid_inum(es, ino)) {
    return -1;
  }

  // add code here

  return 0;
}
