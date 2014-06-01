/**
 * extent.c - extent of Ext4.
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
#define DEBUG_LIBEXT4_EXTENT
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
#ifdef DEBUG_LIBEXT4_EXTENT
static char buf[EXT4_SHOW_STAT_EXTENT_SZ];
#endif

/*
 * Function Declaration
 */

/*
 * Function Definition
 */
int32_t ext4_ext_node_header(struct inode *inode, struct ext4_extent_idx *ei, struct ext4_extent_header *eh)
{
  struct super_block *sb = inode->i_sb;
  int64_t offset;
  int32_t ret = 0;

  if (!ei) {
    memcpy((void *)eh, (const void *)inode->i_block, sizeof(struct ext4_extent_header));
  } else {
    offset = (((uint64_t)ei->ei_leaf_hi << 32) | (uint64_t)ei->ei_leaf_lo) * sb->s_blocksize;

    ret = io_seek(offset);
    if (ret != 0) {
      return -1;
    }

    ret = io_read((uint8_t *)eh, sizeof(struct ext4_extent_header));
    if (ret != 0) {
      return -1;
    }
  }

#ifdef DEBUG_LIBEXT4_EXTENT
  memset((void *)buf, 0, sizeof(buf));
  ext4_show_stat_extent_header(eh, buf, sizeof(buf));
  fprintf(stdout, "%s", buf);
#endif

  return 0;
}

int32_t ext4_ext_node_is_leaf(struct ext4_extent_header *eh)
{
  return (eh->eh_depth == 0);
}

int32_t ext4_ext_node_num(struct ext4_extent_header *eh, uint16_t *nodes_num)
{
  *nodes_num = eh->eh_entries;

  return 0;
}

int32_t ext4_ext_index_node(struct inode *inode, struct ext4_extent_idx *ei, struct ext4_extent_idx *nodes, uint16_t nodes_num)
{
  struct super_block *sb = inode->i_sb;
  uint8_t *ptr = NULL;
  int64_t offset;
  uint16_t i;
  int32_t ret = 0;

  if (!ei) {
    ptr = (uint8_t *)((uint8_t *)(inode->i_block) + sizeof(struct ext4_extent_header));

    for (i = 0; i < nodes_num; ++i) {
      memcpy((void *)&nodes[i], (const void *)ptr, sizeof(struct ext4_extent_idx));
      ptr += sizeof(struct ext4_extent_idx);
    }
  } else {
    offset = (((uint64_t)ei->ei_leaf_hi << 32) | (uint64_t)ei->ei_leaf_lo) * sb->s_blocksize + sizeof(struct ext4_extent_header);

    ret = io_seek(offset);
    if (ret != 0) {
      return -1;
    }

    for (i = 0; i < nodes_num; ++i) {
      ret = io_read((uint8_t *)&nodes[i], sizeof(struct ext4_extent_idx));
      if (ret != 0) {
        break;
      }

      offset += sizeof(struct ext4_extent_idx);
      ret = io_seek(offset);
      if (ret != 0) {
        break;
      }
    }
  }

#ifdef DEBUG_LIBEXT4_EXTENT
  for (i = 0; i < nodes_num; ++i) {
    memset((void *)buf, 0, sizeof(buf));
    ext4_show_stat_extent_idx(&nodes[i], buf, sizeof(buf));
    fprintf(stdout, "%s", buf);
  }
#endif

  return ret;
}

int32_t ext4_ext_leaf_node(struct inode *inode, struct ext4_extent_idx *ei, struct ext4_extent *nodes, uint16_t nodes_num)
{
  struct super_block *sb = inode->i_sb;
  uint8_t *ptr = NULL;
  int64_t offset;
  uint16_t i;
  int32_t ret = 0;

  if (!ei) {
    ptr = (uint8_t *)((uint8_t *)(inode->i_block) + sizeof(struct ext4_extent_header));

    for (i = 0; i < nodes_num; ++i) {
      memcpy((void *)&nodes[i], (const void *)ptr, sizeof(struct ext4_extent));
      ptr += sizeof(struct ext4_extent);
    }
  } else {
    offset = (((uint64_t)ei->ei_leaf_hi << 32) | (uint64_t)ei->ei_leaf_lo) * sb->s_blocksize + sizeof(struct ext4_extent_header);

    ret = io_seek(offset);
    if (ret != 0) {
      return -1;
    }

    for (i = 0; i < nodes_num; ++i) {
      ret = io_read((uint8_t *)&nodes[i], sizeof(struct ext4_extent));
      if (ret != 0) {
        break;
      }

      offset += sizeof(struct ext4_extent);
      ret = io_seek(offset);
      if (ret != 0) {
        break;
      }
    }
  }

#ifdef DEBUG_LIBEXT4_EXTENT
  for (i = 0; i < nodes_num; ++i) {
    memset((void *)buf, 0, sizeof(buf));
    ext4_show_stat_extent(&nodes[i], buf, sizeof(buf));
    fprintf(stdout, "%s", buf);
  }
#endif

  return ret;
}
