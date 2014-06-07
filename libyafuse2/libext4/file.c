/**
 * file.c - file of Ext4.
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
#define DEBUG_LIBEXT4_FILE
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
int32_t ext4_get_extent_blk_pos(struct inode *inode, struct ext4_extent *ees, uint16_t num, int64_t offset, uint16_t *index, int64_t *pos);
int32_t ext4_get_extent_file(struct inode *inode, struct ext4_extent *ee, int64_t pos, char *buf, size_t buf_len, int64_t *read_len);
int32_t ext4_traverse_extent_file(struct inode *inode, struct ext4_extent_idx *ei, int64_t offset, char *buf, size_t buf_len, int64_t *read_len);

/*
 * Function Definition
 */
int32_t ext4_get_extent_blk_pos(struct inode *inode, struct ext4_extent *ees, uint16_t num, int64_t offset, uint16_t *index, int64_t *pos)
{
  struct super_block *sb = inode->i_sb;
  int64_t size, len;
  uint16_t i;

  for (i = 0, size = 0; i < num; ++i) {
    len = offset - size;

    if (len >= 0 && len <= (int64_t)sb->s_blocksize) {
      if (len == 0) {
        *index = i + 1;
        *pos = 0;
      } else {
        *index = i;
        *pos = (int64_t)sb->s_blocksize - len;
      }

      break;
    }

    size += (int64_t)(ees[i].ee_len * sb->s_blocksize);
  }

  if (*index >= num) {
    return -1;
  }

  return 0;
}

int32_t ext4_get_extent_file(struct inode *inode, struct ext4_extent *ee, int64_t pos, char *buf, size_t buf_len, int64_t *read_len)
{
  struct super_block *sb = inode->i_sb;
  uint64_t offset;
  int64_t len;
  int32_t ret;

  offset = (((uint64_t)ee->ee_start_hi << 32) | (uint64_t)ee->ee_start_lo) * sb->s_blocksize + pos;
  ret = io_seek(offset);
  if (ret != 0) {
    return -1;
  }

  len = (int64_t)(ee->ee_len * sb->s_blocksize) - pos;
  len = len > (int64_t)buf_len ? (int64_t)buf_len : len;

  ret = io_read((uint8_t *)buf, len);
  if (ret != 0) {
    return -1;
  }

  *read_len = len;

  return 0;
}

int32_t ext4_traverse_extent_file(struct inode *inode, struct ext4_extent_idx *ei, int64_t offset, char *buf, size_t buf_len, int64_t *read_len)
{
  // TODO

  return 0;
}

int32_t ext4_raw_file(struct inode *inode, int64_t offset, char *buf, size_t buf_len, int64_t *read_len)
{
  struct ext4_extent_header eh;
  struct ext4_extent_idx *eis = NULL;
  struct ext4_extent *ees = NULL;
  char *ptr = buf;
  uint16_t num, index, i;
  int64_t pos, curr_len, ret_len, min_len;
  int32_t ret;

  curr_len = buf_len;
  *read_len = 0;
  min_len = inode->i_size > buf_len ? buf_len : inode->i_size;

  ret = ext4_ext_node_header(inode, NULL, &eh);
  if (ret != 0) {
    return -1;
  }

  ret = ext4_ext_node_num(&eh, &num);
  if (ret != 0) {
    return -1;
  }

  if (ext4_ext_node_is_leaf(&eh)) {
    ees = (struct ext4_extent *)malloc(num * sizeof(struct ext4_extent));
    if (!ees) {
      return -1;
    }
    memset((void *)ees, 0, num * sizeof(struct ext4_extent));

    ret = ext4_ext_leaf_node(inode, NULL, ees, num);
    if (ret != 0) {
      goto ext4_raw_file_exit;
    }

    ret = ext4_get_extent_blk_pos(inode, ees, num, offset, &index, &pos);
    if (ret != 0) {
      goto ext4_raw_file_exit;
    }

    for (i = index; i < num; ++i) {
      if (i == index) {
        ret = ext4_get_extent_file(inode, &ees[i], pos, ptr, curr_len, &ret_len);
      } else {
        ret = ext4_get_extent_file(inode, &ees[i], 0, ptr, curr_len, &ret_len);
      }

      if (ret != 0) {
        goto ext4_raw_file_exit;
      }

      *read_len += ret_len;
      if (*read_len >= min_len) {
        *read_len = min_len;
        break;
      }

      ptr += ret_len;
      curr_len -= ret_len;
    }
  } else {
    eis = (struct ext4_extent_idx *)malloc(num * sizeof(struct ext4_extent_idx));
    if (!eis) {
      return -1;
    }
    memset((void *)eis, 0, num * sizeof(struct ext4_extent_idx));

    ret = ext4_ext_index_node(inode, NULL, eis, num);
    if (ret != 0) {
      goto ext4_raw_file_exit;
    }

    for (i = 0; i < num; ++i) {
      ret = ext4_traverse_extent_file(inode, &eis[i], offset, ptr, curr_len, &ret_len);
      if (ret != 0) {
        goto ext4_raw_file_exit;
      }

      *read_len += ret_len;
      if (*read_len >= min_len) {
        *read_len = min_len;
        break;
      }

      ptr += ret_len;
      curr_len -= ret_len;
    }
  }

  ret = 0;

ext4_raw_file_exit:

  if (ees) {
    free((void *)ees);
    ees = NULL;
  }

  if (eis) {
    free((void *)eis);
    eis = NULL;
  }

  return ret;
}

int32_t ext4_raw_link(struct inode *inode, int64_t offset, char *buf, size_t buf_len, int64_t *read_len)
{
  // TODO
  return 0;
}
