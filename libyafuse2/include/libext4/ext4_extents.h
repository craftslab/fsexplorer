/**
 * ext4_extents.h - The header of ext4 extents.
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

#ifndef _EXT4_EXTENTS
#define _EXT4_EXTENTS

#include "ext4.h"

#define AGGRESSIVE_TEST_

#define EXTENTS_STATS__

#define CHECK_BINSEARCH__

#define EXT_DEBUG__

#ifdef CMAKE_COMPILER_IS_GNUCC
#ifdef EXT_DEBUG
#define ext_debug(a...) printk(a)
#else
#define ext_debug(a...)
#endif
#else
#ifdef EXT_DEBUG
#define ext_debug(...) printk(__VA_ARGS__)
#else
#define ext_debug(...)
#endif
#endif /* CMAKE_COMPILER_IS_GNUCC */

#define EXT_STATS_

struct ext4_extent {
 __le32 ee_block;
 __le16 ee_len;
 __le16 ee_start_hi;
 __le32 ee_start_lo;
};

struct ext4_extent_idx {
 __le32 ei_block;
 __le32 ei_leaf_lo;
 __le16 ei_leaf_hi;
 __u16 ei_unused;
};

struct ext4_extent_header {
 __le16 eh_magic;
 __le16 eh_entries;
 __le16 eh_max;
 __le16 eh_depth;
 __le32 eh_generation;
};

#define EXT4_EXT_MAGIC 0xf30a

/*
 * Incomplete definition from kernel/fs/ext4/ext4_extents.h
 */
struct ext4_ext_path {
 __u16 p_depth;
 struct ext4_extent *p_ext;
 struct ext4_extent_idx *p_idx;
 struct ext4_extent_header *p_hdr;
};

#define EXT4_EXT_CACHE_NO 0
#define EXT4_EXT_CACHE_GAP 1
#define EXT4_EXT_CACHE_EXTENT 2

#define EXT_CONTINUE 0
#define EXT_BREAK 1
#define EXT_REPEAT 2

#define EXT_MAX_BLOCK 0xffffffff

#define EXT_INIT_MAX_LEN (1UL << 15)
#define EXT_UNINIT_MAX_LEN (EXT_INIT_MAX_LEN - 1)

#define EXT_FIRST_EXTENT(__hdr__)   ((struct ext4_extent *) (((char *) (__hdr__)) +   sizeof(struct ext4_extent_header)))
#define EXT_FIRST_INDEX(__hdr__)   ((struct ext4_extent_idx *) (((char *) (__hdr__)) +   sizeof(struct ext4_extent_header)))
#define EXT_HAS_FREE_INDEX(__path__)   (le16_to_cpu((__path__)->p_hdr->eh_entries)   < le16_to_cpu((__path__)->p_hdr->eh_max))
#define EXT_LAST_EXTENT(__hdr__)   (EXT_FIRST_EXTENT((__hdr__)) + le16_to_cpu((__hdr__)->eh_entries) - 1)
#define EXT_LAST_INDEX(__hdr__)   (EXT_FIRST_INDEX((__hdr__)) + le16_to_cpu((__hdr__)->eh_entries) - 1)
#define EXT_MAX_EXTENT(__hdr__)   (EXT_FIRST_EXTENT((__hdr__)) + le16_to_cpu((__hdr__)->eh_max) - 1)
#define EXT_MAX_INDEX(__hdr__)   (EXT_FIRST_INDEX((__hdr__)) + le16_to_cpu((__hdr__)->eh_max) - 1)

#endif
