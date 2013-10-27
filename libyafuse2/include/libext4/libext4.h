/**
 * libext4.h - The header of libext4.
 *
 * Copyright (c) 2013-2014 angersax@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program (in the main directory of the distribution
 * in the file COPYING); if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _LIBEXT4_H
#define _LIBEXT4_H

#include "config.h"
#include <stdint.h>

#ifdef DEBUG
// Add code here
#endif

#include "include/fs.h"
#include "include/libext4/ext4.h"
#include "include/libext4/ext4_extents.h"
#include "include/libext4/ext4_jbd2.h"
#include "include/libext4/jbd2.h"

/*
 * Macro Definition
 */

/*
 * Type Definition
 */

/*
 * Function Declaration
 */
int32_t ext4_raw_dentry(struct inode *inode, struct ext4_dir_entry_2 *dentry);

int32_t ext4_raw_inode(struct super_block *sb, uint64_t ino, struct ext4_inode *inode);

int32_t ext4_bg_has_super(struct super_block *sb, ext4_group_t bg);
int32_t ext4_raw_group_desc(struct super_block *sb, ext4_group_t bg, struct ext4_group_desc *gdp);

int32_t ext4_fill_super_info(struct super_block *sb, struct ext4_super_block *es, struct ext4_sb_info *info);
int32_t ext4_raw_super(struct ext4_super_block *es);

#endif /* _LIBEXT4_H */
