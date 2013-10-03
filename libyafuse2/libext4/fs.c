/**
 * fs.c - The filesystem entry of Ext4.
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

#include "config.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <math.h>

#ifdef DEBUG
// Add code here
#endif

#include "include/base/debug.h"
#include "include/base/types.h"
#include "include/libio/io.h"
#include "include/fs.h"
#include "include/libext4/ext4.h"
#include "include/libext4/ext4_extents.h"
#include "include/libext4/ext4_jbd2.h"
#include "include/libext4/jbd2.h"
#include "include/libext4/libext4.h"

/*
 * Macro Definition
 */
#define init_name_hash() 0

/*
 * Type Definition
 */

/*
 * Global Variable Definition
 */
static struct file_system_type fs_file_type;
static struct super_block fs_sb;

/*
 * Function Declaration
 */
static int32_t fs_max_size(uint8_t blocksize_bits);
static inline uint32_t partial_name_hash(uint32_t c, uint32_t prevhash);
static inline uint32_t end_name_hash(uint32_t hash);
static uint32_t fs_name_hash(const unsigned char *name, uint32_t len);

static struct inode* fs_alloc_inode(struct super_block *sb);
static void fs_destroy_inode(struct inode *inode);
static int32_t fs_statfs(struct dentry *dentry, struct kstatfs *buf);
static struct inode* fs_instantiate_inode(struct inode *inode, uint32_t ino);

static int32_t fs_d_hash(const struct dentry *dentry, const struct inode *inode, struct qstr *qstr);
static void fs_d_release(struct dentry *dentry);
static char* fs_d_dname(struct dentry *dentry, char *buffer, int32_t buflen);
static struct dentry* fs_alloc_dentry_intern(struct super_block *sb, struct qstr *name);
static struct dentry* fs_instantiate_dentry(struct dentry *dentry, struct inode *inode);
static struct dentry* fs_make_root(struct super_block *sb, const char *name);

static int32_t fs_fill_super(struct super_block *sb, const char *name);

static struct dentry* fs_mount(struct file_system_type *type, int32_t flags,
                               const char *name, void *data);
static int32_t fs_umount(const char *name, int32_t flags);

static struct super_operations fs_super_opt = {
  fs_alloc_inode,
  fs_destroy_inode,
  fs_statfs,
};

static struct dentry_operations fs_dentry_opt = {
  fs_d_hash,
  fs_d_release,
  fs_d_dname,
};

/*
 * Function Definition
 */
/*
 * Maximal file size
 */
static int32_t fs_max_size(uint8_t blocksize_bits)
{
  int32_t res;

  res = (1LL << 32) - 1;
  res <<= blocksize_bits;

  return res;
}

/*
 * Hash partial name
 */
static inline uint32_t partial_name_hash(uint32_t c, uint32_t prevhash)
{
  return (prevhash + (c << 4) + (c >> 4)) * 11;
}

/*
 * Hash end name
 */
static inline uint32_t end_name_hash(uint32_t hash)
{
  return hash;
}

/*
 * Hash name
 */
static uint32_t fs_name_hash(const unsigned char *name, uint32_t len)
{
  uint32_t hash = init_name_hash();

  while (len--) {
    hash = partial_name_hash(*name++, hash);
  }

  return end_name_hash(hash);
}

/*
 * Allocate inode
 */
static struct inode* fs_alloc_inode(struct super_block *sb)
{
  return NULL;
}

/*
 * Destroy inode
 */
static void fs_destroy_inode(struct inode *inode)
{
  return;
}

/*
 * Status of filesystem
 */
static int32_t fs_statfs(struct dentry *dentry, struct kstatfs *buf)
{
  return 0;
}

/*
 * Instantiate inode
 */
static struct inode* fs_instantiate_inode(struct inode *inode, uint32_t ino)
{
  return NULL;
}

/*
 * Hash dentry
 */
static int32_t fs_d_hash(const struct dentry *dentry, const struct inode *inode, struct qstr *qstr)
{
  return 0;
}

/*
 * Release dentry
 */
static void fs_d_release(struct dentry *dentry)
{
  return;
}

/*
 * Name of dentry
 */
static char* fs_d_dname(struct dentry *dentry, char *buffer, int32_t buflen)
{
  return 0;
}

/*
 * Allocate dentry internally
 */
static struct dentry* fs_alloc_dentry_intern(struct super_block *sb, struct qstr *name)
{
  return NULL;
}

/*
 * Instantiate dentry
 */
static struct dentry* fs_instantiate_dentry(struct dentry *dentry, struct inode *inode)
{
  return NULL;
}

/*
 * Make dentry of root
 */
static struct dentry* fs_make_root(struct super_block *sb, const char *name)
{
  struct inode *root_inode = NULL;
  struct qstr q_name;
  struct dentry *root_dentry = NULL;

  root_inode = sb->s_op->alloc_inode(sb);
  if (!root_inode) {
    return NULL;
  }

  root_inode = fs_instantiate_inode(root_inode, EXT4_ROOT_INO);
  if (!root_inode) {
    goto fs_make_root_fail;
  }

  memset((void *)&q_name, 0, sizeof(struct qstr));
  q_name.name = (const unsigned char *)name;
  q_name.len = strlen((const char *)(q_name.name));
  q_name.hash = fs_name_hash(q_name.name, q_name.len);

  root_dentry = fs_alloc_dentry_intern(root_inode->i_sb, &q_name);
  if (!root_dentry) {
    goto fs_make_root_fail;
  }

  root_dentry = fs_instantiate_dentry(root_dentry, root_inode);
  if (!root_dentry) {
    goto fs_make_root_fail;
  }

  return root_dentry;

 fs_make_root_fail:

  if (root_dentry) {
    sb->s_d_op->d_release(root_dentry);
  }

  if (root_inode) {
    sb->s_op->destroy_inode(root_inode);
  }

  return NULL;
}

/*
 * Fill in superblock
 */
static int32_t fs_fill_super(struct super_block *sb, const char *name)
{
  struct ext4_super_block ext4_sb;
  uint32_t len;
  int32_t ret;

  /*
   * Fill in Ext4 superblock
   */
  memset((void *)&ext4_sb, 0, sizeof(struct ext4_super_block));
  ret = ext4_fill_sb(&ext4_sb);
  if (ret != 0) {
    return -1;
  }

  /*
   * Fill in superblock
   */
  sb->s_blocksize_bits = (uint8_t)ext4_sb.s_log_block_size;
  sb->s_blocksize = (uint32_t)pow((double)2, (double)(10 + ext4_sb.s_log_block_size));
  sb->s_maxbytes = (int32_t)fs_max_size(sb->s_blocksize_bits);
  sb->s_type = (struct file_system_type *)&fs_file_type;
  sb->s_op = (const struct super_operations *)&fs_super_opt;
  sb->s_flags = (uint32_t)ext4_sb.s_flags;
  sb->s_magic = (uint32_t)ext4_sb.s_magic;
  sb->s_count = (int32_t)1;

  len = (uint32_t)(sizeof(sb->s_id) >= (strlen(fs_file_type.name) + 1) ? strlen(fs_file_type.name) + 1 : sizeof(sb->s_id));
  memcpy((void *)(sb->s_id), (const void *)(fs_file_type.name), len);
  sb->s_id[len - 1] = '\0';

  len = (uint32_t)(sizeof(sb->s_uuid) >= sizeof(ext4_sb.s_uuid) ? sizeof(ext4_sb.s_uuid) : sizeof(sb->s_uuid));
  memcpy((void *)(sb->s_uuid), (const void *)(ext4_sb.s_uuid), len);

  sb->s_d_op = (const struct dentry_operations *)&fs_dentry_opt;
  sb->s_root = (struct dentry *)fs_make_root(sb, name);
  //sb->s_inodes = ; // add code here

  return 0;
}

/*
 * Mount filesystem
 */
static struct dentry* fs_mount(struct file_system_type *type, int32_t flags,
                               const char *name, void *data)
{
  int32_t ret;

  if (!type || !name) {
    return NULL;
  }

  /*
   * Open filesystem
   */
  ret = io_open(name);
  if (ret != 0) {
    return NULL;
  }

  /*
   * Fill in superblock
   */
  memset((void *)&fs_sb, 0, sizeof(struct super_block));
  ret = fs_fill_super(&fs_sb, name);
  if (ret != 0) {
    goto fs_mount_fail;
  }

  return NULL;

 fs_mount_fail:

  memset((void *)&fs_sb, 0, sizeof(struct super_block));

  (void)io_close();

  return NULL;
}

/*
 * Unmount filesystem
 */
static int32_t fs_umount(const char *name, int32_t flags)
{
  if (!name) {
    return -1;
  }

  /*
   * Free list of dentry
   */
  // add code here

  /*
   * Free list of inode
   */
  // add code here

  memset((void *)&fs_sb, 0, sizeof(struct super_block));

  (void)io_close();

  return 0;
}

/*
 * Init filesystem type
 */
#ifdef CMAKE_COMPILER_IS_GNUCC
struct file_system_type* fs_file_system_type_init(const char *type, int32_t flags)
#else
__declspec(dllexport) struct file_system_type* fs_file_system_type_init(const char *type, int32_t flags)
#endif /* CMAKE_COMPILER_IS_GNUCC */
{
  if (!type) {
    return NULL;
  }

  memset((void *)&fs_file_type, 0, sizeof(struct file_system_type));
  fs_file_type.name = type;
  fs_file_type.fs_flags = flags;
  fs_file_type.mount = fs_mount;
  fs_file_type.umount = fs_umount;

  return &fs_file_type;
}
