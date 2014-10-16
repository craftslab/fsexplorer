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
static char buf[EXT4_SHOW_STAT_DENTRY_SZ];
#endif

/*
 * Function Declaration
 */
static int32_t ext4_find_dentry(struct inode *inode, uint64_t offset, struct ext4_dir_entry_2 *dentry);
static int32_t ext4_get_dents_num(struct inode *inode, uint64_t offset, uint32_t *dents_num);
static int32_t ext4_get_dents(struct inode *inode, uint64_t offset, struct ext4_dir_entry_2 *dents, uint32_t *dents_index);
static int32_t ext4_get_direct_dents_num(struct inode *inode, uint32_t *dents_num);
static int32_t ext4_get_direct_dents(struct inode *inode, struct ext4_dir_entry_2 *dents, uint32_t *dents_index, uint32_t dents_num);
static int32_t ext4_get_extent_dents_num(struct inode *inode, struct ext4_extent *ee, uint32_t *dents_num);
static int32_t ext4_get_extent_dents(struct inode *inode, struct ext4_extent *ee, struct ext4_dir_entry_2 *dents, uint32_t *dents_index);
static int32_t ext4_traverse_extent_dents_num(struct inode *inode, struct ext4_extent_idx *ei, uint32_t *dents_num);
static int32_t ext4_traverse_extent_dents(struct inode *inode, struct ext4_extent_idx *ei, struct ext4_dir_entry_2 *dents, uint32_t *dents_index, uint32_t dents_num);

/*
 * Function Definition
 */
static int32_t ext4_find_dentry(struct inode *inode, uint64_t offset, struct ext4_dir_entry_2 *dentry)
{
  int64_t len;
  int32_t ret;

  ret = io_seek(offset);
  if (ret != 0) {
    return -1;
  }

  ret = io_read((uint8_t *)&dentry->inode, sizeof(dentry->inode));
  if (ret != 0) {
    return -1;
  }

  offset += sizeof(dentry->inode);
  ret = io_seek(offset);
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

  offset += sizeof(dentry->rec_len);
  ret = io_seek(offset);
  if (ret != 0) {
    return -1;
  }

  ret = io_read((uint8_t *)dentry + sizeof(dentry->inode) + sizeof(dentry->rec_len), len);
  if (ret != 0) {
    return -1;
  }

  return 0;
}

static int32_t ext4_get_dents_num(struct inode *inode, uint64_t offset, uint32_t *dents_num)
{
  struct ext4_dir_entry_2 dentry;
  uint32_t i;
  int32_t ret;

  i = 0;

  while (1) {
    memset((void *)&dentry, 0, sizeof(struct ext4_dir_entry_2));
    ret = ext4_find_dentry(inode, offset, &dentry);
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

  *dents_num = i;

  return ret;
}

static int32_t ext4_get_dents(struct inode *inode, uint64_t offset, struct ext4_dir_entry_2 *dents, uint32_t *dents_index)
{
  struct ext4_dir_entry_2 dentry;
  uint32_t i = *dents_index;
  int32_t ret;

  while (1) {
    memset((void *)&dentry, 0, sizeof(struct ext4_dir_entry_2));
    ret = ext4_find_dentry(inode, offset, &dentry);
    if (ret != 0) {
      break;
    }

    if (dentry.inode == EXT4_UNUSED_INO) {
      ret = 0;
      break;
    }

    dents[i++] = dentry;

    offset += dentry.rec_len <= sizeof(struct ext4_dir_entry_2) ? dentry.rec_len : sizeof(struct ext4_dir_entry_2);

#ifdef DEBUG_LIBEXT4_DIR
    memset((void *)buf, 0, sizeof(buf));
    ext4_show_stat_dentry(&dents[i - 1], buf, sizeof(buf));
    fprintf(stdout, "%s", buf);
#endif
  }

  *dents_index = i;

  return ret;
}

static int32_t ext4_get_direct_dents_num(struct inode *inode, uint32_t *dents_num)
{
  struct super_block *sb = inode->i_sb;
  uint32_t num;
  uint64_t offset;
  int32_t i;
  int32_t ret;

  if (inode->i_block[EXT4_IND_BLOCK]
      || inode->i_block[EXT4_DIND_BLOCK]
      || inode->i_block[EXT4_TIND_BLOCK]) {
    // TODO
    return -1;
  }

  *dents_num = 0;

  for (i = 0; i < EXT4_NDIR_BLOCKS; ++i) {
    offset = (uint64_t)inode->i_block[i] * sb->s_blocksize;
    ret = ext4_get_dents_num(inode, offset, &num);
    if (ret != 0) {
      break;
    }

    *dents_num += num;
  }

  return ret;
}

static int32_t ext4_get_direct_dents(struct inode *inode, struct ext4_dir_entry_2 *dents, uint32_t *dents_index, uint32_t dents_num)
{
  struct super_block *sb = inode->i_sb;
  uint64_t offset;
  int32_t i;
  int32_t ret;

  if (inode->i_block[EXT4_IND_BLOCK]
      || inode->i_block[EXT4_DIND_BLOCK]
      || inode->i_block[EXT4_TIND_BLOCK]) {
    // TODO
    return -1;
  }

  for (i = 0; i < EXT4_NDIR_BLOCKS && *dents_index < dents_num; ++i) {
    offset = (uint64_t)inode->i_block[i] * sb->s_blocksize;
    ret = ext4_get_dents(inode, offset, dents, dents_index);
    if (ret != 0) {
      break;
    }
  }

  return ret;
}

static int32_t ext4_get_extent_dents_num(struct inode *inode, struct ext4_extent *ee, uint32_t *dents_num)
{
  struct super_block *sb = inode->i_sb;
  uint64_t offset;

  offset = (((uint64_t)ee->ee_start_hi << 32) | (uint64_t)ee->ee_start_lo) * sb->s_blocksize;

  return ext4_get_dents_num(inode, offset, dents_num);
}

static int32_t ext4_get_extent_dents(struct inode *inode, struct ext4_extent *ee, struct ext4_dir_entry_2 *dents, uint32_t *dents_index)
{
  struct super_block *sb = inode->i_sb;
  uint64_t offset = 0;

  offset = (((uint64_t)ee->ee_start_hi << 32) | (uint64_t)ee->ee_start_lo) * sb->s_blocksize;

  return ext4_get_dents(inode, offset, dents, dents_index);
}

static int32_t ext4_traverse_extent_dents_num(struct inode *inode, struct ext4_extent_idx *ei, uint32_t *dents_num)
{
  struct ext4_extent_header eh;
  struct ext4_extent_idx *eis = NULL;
  struct ext4_extent *ees = NULL;
  uint16_t nodes_num, i;
  uint32_t num;
  int32_t ret;

  ret = ext4_ext_node_header(inode, ei, &eh);
  if (ret != 0) {
    return -1;
  }

  ret = ext4_ext_node_num(&eh, &nodes_num);
  if (ret != 0) {
    return -1;
  }

  if (ext4_ext_node_is_leaf(&eh)) {
    ees = (struct ext4_extent *)malloc(nodes_num * sizeof(struct ext4_extent));
    if (!ees) {
      return -1;
    }
    memset((void *)ees, 0, nodes_num * sizeof(struct ext4_extent));

    ret = ext4_ext_leaf_node(inode, ei, ees, nodes_num);
    if (ret != 0) {
      goto ext4_traverse_extent_dents_num_exit;
    }

    for (i = 0; i < nodes_num; ++i) {
      ret = ext4_get_extent_dents_num(inode, &ees[i], &num);
      if (ret != 0) {
        goto ext4_traverse_extent_dents_num_exit;
      }

      *dents_num += num;
    }
  } else {
    eis = (struct ext4_extent_idx *)malloc(nodes_num * sizeof(struct ext4_extent_idx));
    if (!eis) {
      return -1;
    }
    memset((void *)eis, 0, nodes_num * sizeof(struct ext4_extent_idx));

    ret = ext4_ext_index_node(inode, ei, eis, nodes_num);
    if (ret != 0) {
      goto ext4_traverse_extent_dents_num_exit;
    }

    for (i = 0; i < nodes_num; ++i) {
      ret = ext4_traverse_extent_dents_num(inode, &eis[i], dents_num);
      if (ret != 0) {
        goto ext4_traverse_extent_dents_num_exit;
      }
    }
  }

  ret = 0;

ext4_traverse_extent_dents_num_exit:

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

static int32_t ext4_traverse_extent_dents(struct inode *inode, struct ext4_extent_idx *ei, struct ext4_dir_entry_2 *dents, uint32_t *dents_index, uint32_t dents_num)
{
  struct ext4_extent_header eh;
  struct ext4_extent_idx *eis = NULL;
  struct ext4_extent *ees = NULL;
  uint16_t nodes_num, i;
  int32_t ret;

  ret = ext4_ext_node_header(inode, ei, &eh);
  if (ret != 0) {
    return -1;
  }

  ret = ext4_ext_node_num(&eh, &nodes_num);
  if (ret != 0) {
    return -1;
  }

  if (ext4_ext_node_is_leaf(&eh)) {
    ees = (struct ext4_extent *)malloc(nodes_num * sizeof(struct ext4_extent));
    if (!ees) {
      return -1;
    }
    memset((void *)ees, 0, nodes_num * sizeof(struct ext4_extent));

    ret = ext4_ext_leaf_node(inode, ei, ees, nodes_num);
    if (ret != 0) {
      goto ext4_traverse_extent_dents_exit;
    }

    for (i = 0; i < nodes_num && *dents_index < dents_num; ++i) {
      ret = ext4_get_extent_dents(inode, &ees[i], dents, dents_index);
      if (ret != 0) {
        goto ext4_traverse_extent_dents_exit;
      }
    }
  } else {
    eis = (struct ext4_extent_idx *)malloc(nodes_num * sizeof(struct ext4_extent_idx));
    if (!eis) {
      return -1;
    }
    memset((void *)eis, 0, nodes_num * sizeof(struct ext4_extent_idx));

    ret = ext4_ext_index_node(inode, ei, eis, nodes_num);
    if (ret != 0) {
      goto ext4_traverse_extent_dents_exit;
    }

    for (i = 0; i < nodes_num && *dents_index < dents_num; ++i) {
      ret = ext4_traverse_extent_dents(inode, &eis[i], dents, dents_index, dents_num);
      if (ret != 0) {
        goto ext4_traverse_extent_dents_exit;
      }
    }
  }

  ret = 0;

ext4_traverse_extent_dents_exit:

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

int32_t ext4_raw_dentry_num(struct dentry *parent, uint32_t *childs_num)
{
  struct inode *inode = parent->d_inode;
  int32_t ret;

  /*
   * Support for linear directories only
   * and hash tree directories are NOT supported yet
   */
  if (is_dx(inode)) {
    return -1;
  }

  *childs_num = 0;

  if (ext4_ext_header_check(inode) == 0) {
    ret = ext4_traverse_extent_dents_num(inode, NULL, childs_num);
  } else {
    ret = ext4_get_direct_dents_num(inode, childs_num);
  }

  return ret;
}

int32_t ext4_raw_dentry(struct dentry *parent, struct ext4_dir_entry_2 *childs, uint32_t childs_num)
{
  struct inode *inode = parent->d_inode;
  uint32_t childs_index;
  int32_t ret;

  /*
   * Support for linear directories only
   * and hash tree directories are NOT supported yet
   */
  if (is_dx(inode)) {
    return -1;
  }

  parent->d_childnum = childs_num;
  childs_index = 0;

  if (ext4_ext_header_check(inode) == 0) {
    ret = ext4_traverse_extent_dents(inode, NULL, childs, &childs_index, childs_num);
  } else {
    ret = ext4_get_direct_dents(inode, childs, &childs_index, childs_num);
  }

  return ret;
}
