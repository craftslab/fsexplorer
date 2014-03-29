/**
 * libext4.h - The header of libext4.
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

#ifndef _LIBEXT4_H
#define _LIBEXT4_H

#include "config.h"
#include <stdint.h>

#ifdef DEBUG
#define DEBUG_INCLUDE_LIBEXT4_LIBEXT4
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
#ifdef DEBUG_INCLUDE_LIBEXT4_LIBEXT4
void ext4_show_dentry(struct ext4_dir_entry_2 *dentry);
void ext4_show_extent(struct ext4_extent *ext);
void ext4_show_extent_idx(struct ext4_extent_idx *ei);
void ext4_show_extent_header(struct ext4_extent_header *eh);
void ext4_show_inode_stat(struct ext4_super_block *sb, uint64_t ino, struct ext4_inode *inode);
void ext4_show_gdp_stat(struct ext4_super_block *sb, ext4_group_t bg, struct ext4_group_desc *gdp);
void ext4_show_sb_stat(struct ext4_super_block *sb);
#endif

int32_t ext4_raw_dentry_num(struct dentry *parent, uint32_t *childs_num);
int32_t ext4_raw_dentry(struct dentry *parent, struct ext4_dir_entry_2 *childs, uint32_t childs_num);

int32_t ext4_ext_depth(struct inode *inode, uint16_t *depth);
int32_t ext4_ext_find_extent(struct inode *inode, uint16_t depth, struct ext4_ext_path *path);

int32_t ext4_inode_hdr(struct inode *inode, struct ext4_extent_header *eh);
int32_t ext4_raw_inode(struct super_block *sb, uint64_t ino, struct ext4_inode *inode);

int32_t ext4_bg_has_super(struct super_block *sb, ext4_group_t bg);

#if 0 // DISUSED here
int32_t ext4_raw_group_desc(struct super_block *sb, ext4_group_t bg, struct ext4_group_desc *gdp);
#else
int32_t ext4_raw_group_desc(struct super_block *sb, uint32_t bg_cnt, struct ext4_group_desc *gdp);
#endif

int32_t ext4_fill_super_info(struct super_block *sb, struct ext4_super_block *es, struct ext4_sb_info *info);
int32_t ext4_raw_super(struct ext4_super_block *es);

#endif /* _LIBEXT4_H */
