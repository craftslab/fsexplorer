/**
 * fs.c - The filesystem entry of Ext4.
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
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <math.h>

#ifdef DEBUG
#define DEBUG_LIBEXT4_FS
#endif

#include "include/base/debug.h"
#include "include/base/types.h"
#include "include/libio/io.h"
#include "include/fs.h"
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

static char fs_stat_sb[EXT4_SHOW_STAT_SB_SZ];
static char fs_stat_inode[EXT4_SHOW_STAT_INODE_SZ];

/*
 * Function Declaration
 */
static int32_t fs_max_size(uint8_t blocksize_bits);
static inline uint64_t partial_name_hash(uint64_t c, uint64_t prevhash);
static inline uint64_t end_name_hash(uint64_t hash);
static uint64_t fs_name_hash(const unsigned char *name, uint32_t len);

static struct dentry* fs_alloc_dentry(struct super_block *sb);
static struct dentry* fs_alloc_dentry_child(struct dentry *parent);
static void fs_d_release(struct dentry *dentry);
static struct dentry* fs_instantiate_dentry(struct dentry *dentry, struct inode *inode, const unsigned char *name, uint8_t name_len);

static struct inode* fs_alloc_inode(struct super_block *sb);
static void fs_destroy_inode(struct inode *inode);
static void fs_destroy_inodes(struct super_block *sb);
static struct inode* fs_find_inode(struct super_block *sb, uint64_t ino);
static struct inode* fs_instantiate_inode(struct inode *inode, uint64_t ino);

static struct dentry* fs_create_parent(struct super_block *sb, uint64_t ino, const unsigned char *name, uint8_t name_len);
static struct dentry* fs_create_child(struct super_block *sb, struct dentry *parent, uint64_t ino, const unsigned char *name, uint8_t name_len);
static struct dentry* fs_make_root(struct super_block *sb);
static int32_t fs_fill_super(struct super_block *sb);

static struct dentry* fs_mount(struct file_system_type *type, uint64_t flags, const char *name, void *data);
static int32_t fs_umount(const char *name, int32_t flags);
static int32_t fs_traverse_dentry(struct dentry **dentry);
static int32_t fs_statfs(struct dentry *dentry, struct kstatfs *buf);
static int32_t fs_statrawfs(struct dentry *dentry, const char **buf);
static int32_t fs_statraw(struct inode *inode, const char **buf);
static int32_t fs_open(struct inode *inode, struct file *file);
static int32_t fs_release(struct inode *inode, struct file *file);
static ssize_t fs_readat(struct file *file, int64_t offset, char *buf, size_t count, int64_t *num);

static struct dentry_operations fs_dentry_opt = {
  //.d_hash =
  NULL,

  //.d_release =
  fs_d_release,

  //.d_dname =
  NULL,
};

static struct inode_operations fs_inode_opt = {
  //.lookup =
  NULL,

  //.permission =
  NULL,

  //.get_acl =
  NULL,

  //.create =
  NULL,

  //.link =
  NULL,

  //.unlink =
  NULL,

  //.symlink =
  NULL,

  //.mkdir =
  NULL,

  //.rmdir =
  NULL,

  //.mknod =
  NULL,

  //.rename =
  NULL,

  //.setattr =
  NULL,

  //.getattr =
  NULL,

  //.setxattr =
  NULL,

  //.getxattr =
  NULL,

  //.listxattr =
  NULL,

  //.removexattr =
  NULL,

  //.update_time =
  NULL,
};

static struct super_operations fs_super_opt = {
  //.alloc_inode =
  fs_alloc_inode,

  //.destroy_inode =
  fs_destroy_inode,

  //.traverse_dentry =
  fs_traverse_dentry,

  //.statfs =
  fs_statfs,

  //.statrawfs =
  fs_statrawfs,

  //.statraw =
  fs_statraw,
};

static struct file_operations fs_file_opt = {
  //.llseek =
  NULL,

  //.read =
  NULL,

  //.write =
  NULL,

  //.open =
  fs_open,

  //.release =
  fs_release,

  //.readat =
  fs_readat,
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
static inline uint64_t partial_name_hash(uint64_t c, uint64_t prevhash)
{
  return (prevhash + (c << 4) + (c >> 4)) * 11;
}

/*
 * Hash end name
 */
static inline uint64_t end_name_hash(uint64_t hash)
{
  return hash;
}

/*
 * Hash name
 */
static uint64_t fs_name_hash(const unsigned char *name, uint32_t len)
{
  uint64_t hash = init_name_hash();

  while (len--) {
    hash = partial_name_hash(*name++, hash);
  }

  return end_name_hash(hash);
}

/*
 * Allocate dentry
 */
static struct dentry* fs_alloc_dentry(struct super_block *sb)
{
  struct dentry *dentry = NULL;
  struct qstr *q_name = NULL;

  dentry = (struct dentry *)malloc(sizeof(struct dentry));
  if (!dentry) {
    return NULL;
  }
  memset((void *)dentry, 0, sizeof(struct dentry));

  q_name = (struct qstr *)malloc(sizeof(struct qstr));
  if (!q_name) {
    goto fs_alloc_dentry_fail;
  }
  memset((void *)q_name, 0, sizeof(struct qstr));

  q_name->name = (const unsigned char *)malloc(EXT4_NAME_LEN);
  if (!q_name->name) {
    goto fs_alloc_dentry_fail;
  }
  memset((void *)q_name->name, 0, EXT4_NAME_LEN);

  q_name->len = (uint32_t)(strlen(DNAME_ROOT) > EXT4_NAME_LEN ? EXT4_NAME_LEN : strlen(DNAME_ROOT));
  memcpy((void *)q_name->name, (const void *)DNAME_ROOT, q_name->len);
  q_name->hash = (uint32_t)fs_name_hash(q_name->name, q_name->len);

  dentry->d_parent = dentry;
  dentry->d_name = (struct qstr *)q_name;
  dentry->d_op = sb->s_d_op;
  dentry->d_sb = sb;
  list_init(&dentry->d_child);
  list_init(&dentry->d_subdirs);

  return dentry;

 fs_alloc_dentry_fail:

  if (dentry) {
    if (dentry->d_name->name) {
      free((void *)dentry->d_name->name);
      dentry->d_name->name = NULL;
      dentry->d_name->len = 0;
    }

    if (dentry->d_name) {
      free((void *)dentry->d_name);
      dentry->d_name = NULL;
    }

    free((void *)dentry);
    dentry = NULL;
  }

  return NULL;
}

/*
 * Allocate dentry
 */
static struct dentry* fs_alloc_dentry_child(struct dentry *parent)
{
  struct dentry *dentry = NULL;

  dentry = fs_alloc_dentry(parent->d_sb);
  if (!dentry) {
    return NULL;
  }

  dentry->d_parent = parent;
  list_add(&dentry->d_child, &parent->d_subdirs);

  return dentry;
}

/*
 * Release dentry
 */
static void fs_d_release(struct dentry *dentry)
{
  struct dentry *child = NULL;
  struct list_head *ptr = NULL;

  if (!dentry) {
    return;
  }

  if (!list_empty(&dentry->d_subdirs)) {
#if 0  // For CMAKE_COMPILER_IS_GNUCC only
    list_for_each_entry(child, &dentry->d_subdirs, d_child) {
#else
    for (child = list_entry((&dentry->d_subdirs)->next, struct dentry, d_child);
        &child->d_child != (&dentry->d_subdirs);
        child = list_entry(ptr, struct dentry, d_child)) {
#endif
      ptr = child->d_child.next;
      fs_d_release(child);
    }
  }

  if (dentry->d_name) {
    if (dentry->d_name->name) {
      free((void *)dentry->d_name->name);
      dentry->d_name->name = NULL;
      dentry->d_name->len = 0;
    }
    free((void *)dentry->d_name);
    dentry->d_name = NULL;
  }

  if (dentry) {
    free((void *)dentry);
    dentry = NULL;
  }

  return;
}

/*
 * Instantiate dentry
 */
static struct dentry* fs_instantiate_dentry(struct dentry *dentry, struct inode *inode, const unsigned char *name, uint8_t name_len)
{
  dentry->d_parent = (struct dentry *)dentry->d_parent;

  dentry->d_name->name = (const unsigned char *)malloc(EXT4_NAME_LEN);
  if (!dentry->d_name->name) {
    return NULL;
  }
  memset((void *)dentry->d_name->name, 0, EXT4_NAME_LEN);

  dentry->d_name->len = (uint32_t)(name_len > EXT4_NAME_LEN ? EXT4_NAME_LEN : name_len);
  memcpy((void *)dentry->d_name->name, (const void *)name, dentry->d_name->len);
  dentry->d_name->hash = (uint32_t)fs_name_hash(dentry->d_name->name, dentry->d_name->len);

  dentry->d_inode = (struct inode *)inode;
  dentry->d_op = (const struct dentry_operations *)dentry->d_op;
  dentry->d_sb = (struct super_block *)dentry->d_sb;
  memcpy((void *)&dentry->d_child, (const void *)&dentry->d_child, sizeof(struct list_head));
  memcpy((void *)&dentry->d_subdirs, (const void *)&dentry->d_subdirs, sizeof(struct list_head));

  return dentry;
}

/*
 * Allocate inode
 */
static struct inode* fs_alloc_inode(struct super_block *sb)
{
  struct inode *inode = NULL;

  inode = (struct inode *)malloc(sizeof(struct inode));
  if (!inode) {
    return NULL;
  }
  memset((void *)inode, 0, sizeof(struct inode));

  inode->i_sb = sb;
  list_add(&inode->i_sb_list, &inode->i_sb->s_inodes);

  return inode;
}

/*
 * Destroy inode
 */
static void fs_destroy_inode(struct inode *inode)
{
  if (!inode) {
    return;
  }

  if (inode->i_block) {
    free((void *)inode->i_block);
    inode->i_block = NULL;
  }

  if (inode) {
    free((void *)inode);
    inode = NULL;
  }
}

/*
 * Destroy all inodes
 */
static void fs_destroy_inodes(struct super_block *sb)
{
  struct inode *child = NULL;
  struct list_head *ptr = NULL;

  if (!list_empty(&sb->s_inodes)) {
#if 0  // For CMAKE_COMPILER_IS_GNUCC only
    list_for_each_entry(child, &sb->s_inodes, i_sb_list) {
#else
    for (child = list_entry((&sb->s_inodes)->next, struct inode, i_sb_list);
         &child->i_sb_list != (&sb->s_inodes);
         child = list_entry(ptr, struct inode, i_sb_list)) {
#endif
      ptr = child->i_sb_list.next;

      if (!list_empty(&child->i_sb_list)) {
        list_del_init(&child->i_sb_list);
      }

      sb->s_op->destroy_inode(child);
    }
  }
}

/*
 * Find inode matched with ino
 */
static struct inode* fs_find_inode(struct super_block *sb, uint64_t ino)
{
  struct inode *child = NULL, *ptr = NULL;

  if (!list_empty(&sb->s_inodes)) {
#if 0  // For CMAKE_COMPILER_IS_GNUCC only
    list_for_each_entry(child, &sb->s_inodes, i_sb_list) {
#else
    for (child = list_entry((&sb->s_inodes)->next, struct inode, i_sb_list);
         &child->i_sb_list != (&sb->s_inodes);
         child = list_entry(child->i_sb_list.next, struct inode, i_sb_list)) {
#endif
      if (ino == child->i_ino) {
        ptr = child;
        break;
      }
    }
  }

  return ptr;
}

/*
 * Instantiate inode
 */
static struct inode* fs_instantiate_inode(struct inode *inode, uint64_t ino)
{
  struct super_block *sb = inode->i_sb;
  struct ext4_inode ext4_inode;
  int32_t ret;

  /*
   * Fill in Ext4 inode
   */
  ret = ext4_raw_inode(sb, ino, &ext4_inode);
  if (ret != 0) {
    return NULL;
  }

  /*
   * Fill in inode
   */
  inode->i_mode = (uint16_t)ext4_inode.i_mode;
  inode->i_uid = (uint32_t)ext4_inode.i_uid;
  inode->i_gid = (uint32_t)ext4_inode.i_gid;
  inode->i_flags = (uint32_t)ext4_inode.i_flags;
  inode->i_op = (const struct inode_operations *)&fs_inode_opt;
  inode->i_sb = (struct super_block *)sb;
  inode->i_ino = (uint64_t)ino;

  memset((void *)&inode->i_atime, 0, sizeof(struct fs_timespec));
  inode->i_atime.tv_sec = (int64_t)ext4_inode.i_atime;
  memset((void *)&inode->i_mtime, 0, sizeof(struct fs_timespec));
  inode->i_mtime.tv_sec = (int64_t)ext4_inode.i_mtime;
  memset((void *)&inode->i_ctime, 0, sizeof(struct fs_timespec));
  inode->i_ctime.tv_sec = (int64_t)ext4_inode.i_ctime;

  inode->i_blocks = (uint64_t)(((uint64_t)ext4_inode.osd2.linux2.l_i_blocks_high << 32) | (uint64_t)ext4_inode.i_blocks_lo);
  inode->i_size = (int64_t)(((int64_t)ext4_inode.i_size_high << 32) | (int64_t)ext4_inode.i_size_lo);
  memcpy((void *)&inode->i_sb_list, (void *)&inode->i_sb_list, sizeof(struct list_head));
  inode->i_count = (uint32_t)ext4_inode.i_links_count;
  inode->i_version = (uint64_t)(((uint64_t)ext4_inode.i_version_hi << 32) | (uint64_t)ext4_inode.osd1.linux1.l_i_version);
  inode->i_fop = (const struct file_operations *)&fs_file_opt;

  inode->i_block = (uint32_t *)malloc(EXT4_N_BLOCKS * sizeof(uint32_t));
  if (!inode->i_block) {
    return NULL;
  }
  memset((void *)inode->i_block, 0, EXT4_N_BLOCKS * sizeof(uint32_t));
  memcpy((void *)inode->i_block, (const void *)ext4_inode.i_block, EXT4_N_BLOCKS * sizeof(uint32_t));

  return inode;
}

/*
 * Allocate & instantiate parent inode & dentry
 */
static struct dentry* fs_create_parent(struct super_block *sb, uint64_t ino, const unsigned char *name, uint8_t name_len)
{
  struct inode *inode = NULL;
  struct dentry *dentry = NULL;

  /*
   * Allocate inode
   */
  inode = sb->s_op->alloc_inode(sb);
  if (!inode) {
    return NULL;
  }

  /*
   * Instantiate inode
   */
  inode = fs_instantiate_inode(inode, ino);
  if (!inode) {
    goto fs_create_parent_fail;
  }

  /*
   * Allocate dentry
   */
  dentry = fs_alloc_dentry(inode->i_sb);
  if (!dentry) {
    goto fs_create_parent_fail;
  }

  /*
   * Instantiate dentry
   */
  dentry = fs_instantiate_dentry(dentry, inode, name, name_len);
  if (!dentry) {
    goto fs_create_parent_fail;
  }

  return dentry;

 fs_create_parent_fail:

  if (dentry) {
    sb->s_d_op->d_release(dentry);
    dentry = NULL;
  }

  fs_destroy_inodes(sb);

  return NULL;
}

/*
 * Allocate & instantiate child inode & dentry
 */
static struct dentry* fs_create_child(struct super_block *sb, struct dentry *parent, uint64_t ino, const unsigned char *name, uint8_t name_len)
{
  struct inode *inode = NULL;
  struct dentry *child = NULL;

  inode = fs_find_inode(sb, ino);
  if (!inode) {
    inode = sb->s_op->alloc_inode(sb);
    if (!inode) {
      goto fs_create_child_fail;
    }

    inode = fs_instantiate_inode(inode, ino);
    if (!inode) {
      goto fs_create_child_fail;
    }
  }

  child = fs_alloc_dentry_child(parent);
  if (!child) {
    goto fs_create_child_fail;
  }

  if (!fs_instantiate_dentry(child, inode, name, name_len)) {
    goto fs_create_child_fail;
  }

  return child;

 fs_create_child_fail:

  if (child) {
    sb->s_d_op->d_release(child);
    child = NULL;
  }

  fs_destroy_inodes(sb);

  return NULL;
}

/*
 * Make dentry of root
 */
static struct dentry* fs_make_root(struct super_block *sb)
{
  struct dentry *parent = NULL, *child = NULL;
  struct ext4_dir_entry_2 *ext4_dentries = NULL;
  uint32_t ext4_dentries_num, i;
  struct dentry *ret = NULL;

  /*
   * Allocate & instantiate parent inode & dentry
   */
  parent = fs_create_parent(sb, (uint64_t)EXT4_ROOT_INO, (const unsigned char *)DNAME_ROOT, strlen(DNAME_ROOT));
  if (!parent) {
    ret = NULL;
    goto fs_make_root_fail;
  }

  /*
   * Fill in Ext4 dentry
   */
  ext4_dentries_num = 0;
  if (ext4_raw_dentry_num(parent, &ext4_dentries_num) != 0) {
    ret = NULL;
    goto fs_make_root_fail;
  }

  ext4_dentries = (struct ext4_dir_entry_2 *)malloc(ext4_dentries_num * sizeof(struct ext4_dir_entry_2));
  if (!ext4_dentries) {
    ret = NULL;
    goto fs_make_root_fail;
  }
  memset((void *)ext4_dentries, 0, ext4_dentries_num * sizeof(struct ext4_dir_entry_2));

  if (ext4_raw_dentry(parent, ext4_dentries, ext4_dentries_num) != 0) {
    ret = NULL;
    goto fs_make_root_fail;
  }

  /*
   * Allocate & instantiate child inodes & dentries
   */
  for (i = 0; i < ext4_dentries_num; ++i) {
    child = fs_create_child(sb, parent, (uint64_t)ext4_dentries[i].inode, (const unsigned char *)ext4_dentries[i].name, ext4_dentries[i].name_len);
    if (!child) {
      ret = NULL;
      goto fs_make_root_fail;
    }
  }

  ret = parent;
  goto fs_make_root_exit;

 fs_make_root_fail:

  if (parent) {
    sb->s_d_op->d_release(parent);
    parent = NULL;
  }

  fs_destroy_inodes(sb);

 fs_make_root_exit:

  if (ext4_dentries) {
    free((void *)ext4_dentries);
    ext4_dentries = NULL;
  }

  return ret;
}

/*
 * Fill in superblock
 */
static int32_t fs_fill_super(struct super_block *sb)
{
  struct ext4_super_block ext4_sb;
  uint32_t len;
  int32_t ret;

  /*
   * Fill in Ext4 superblock
   */
  memset((void *)&ext4_sb, 0, sizeof(struct ext4_super_block));
  ret = ext4_raw_super(&ext4_sb);
  if (ret != 0) {
    return -1;
  }

  /*
   * Fill in superblock
   */
  sb->s_blocksize_bits = (uint8_t)ext4_sb.s_log_block_size;
  sb->s_blocksize = (uint64_t)pow((double)2, (double)(10 + ext4_sb.s_log_block_size));
  sb->s_maxbytes = (int64_t)fs_max_size(sb->s_blocksize_bits);
  sb->s_type = (struct file_system_type *)&fs_file_type;
  sb->s_op = (const struct super_operations *)&fs_super_opt;
  sb->s_flags = (uint64_t)ext4_sb.s_flags;
  sb->s_magic = (uint64_t)ext4_sb.s_magic;
  sb->s_count = (int32_t)1;

  len = (uint32_t)(sizeof(sb->s_id) >= (strlen(fs_file_type.name) + 1) ? strlen(fs_file_type.name) + 1 : sizeof(sb->s_id));
  memcpy((void *)(sb->s_id), (const void *)(fs_file_type.name), len);
  sb->s_id[len - 1] = '\0';

  len = (uint32_t)(sizeof(sb->s_uuid) >= sizeof(ext4_sb.s_uuid) ? sizeof(ext4_sb.s_uuid) : sizeof(sb->s_uuid));
  memcpy((void *)(sb->s_uuid), (const void *)(ext4_sb.s_uuid), len);

  sb->s_fs_info = (void *)malloc(sizeof(struct ext4_sb_info));
  if (!sb->s_fs_info) {
    return -1;
  }
  memset((void *)sb->s_fs_info, 0, sizeof(struct ext4_sb_info));
  ret = ext4_fill_super_info(sb, &ext4_sb, (struct ext4_sb_info *)sb->s_fs_info);
  if (ret != 0) {
    goto fs_fill_super_fail;
  }

  sb->s_d_op = (const struct dentry_operations *)&fs_dentry_opt;

  list_init(&sb->s_inodes);
  sb->s_root = (struct dentry *)fs_make_root(sb);
  if (!sb->s_root) {
    ret = -1;
    goto fs_fill_super_fail;
  }

  return 0;

 fs_fill_super_fail:

  if (sb->s_fs_info) {
    free((void *)sb->s_fs_info);
    sb->s_fs_info = NULL;
  }

  return ret;
}

/*
 * Mount filesystem
 */
static struct dentry* fs_mount(struct file_system_type *type, uint64_t flags, const char *name, void *data)
{
  int32_t ret;

  flags = flags;
  data = data;

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
  ret = fs_fill_super(&fs_sb);
  if (ret != 0) {
    goto fs_mount_fail;
  }

  return fs_sb.s_root;

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
  flags = flags;

  if (!name) {
    return -1;
  }

  /*
   * Free list of dentry
   */
  if (fs_sb.s_d_op && fs_sb.s_d_op->d_release && fs_sb.s_root) {
    fs_sb.s_d_op->d_release(fs_sb.s_root);
    fs_sb.s_root = NULL;
  }

  /*
   * Free list of inode
   */
  fs_destroy_inodes(&fs_sb);

  if (((struct ext4_sb_info *)fs_sb.s_fs_info)->s_group_desc) {
    free((void *)((struct ext4_sb_info *)fs_sb.s_fs_info)->s_group_desc);
    ((struct ext4_sb_info *)fs_sb.s_fs_info)->s_group_desc = NULL;
  }

  if (((struct ext4_sb_info *)fs_sb.s_fs_info)->s_es) {
    free((void *)((struct ext4_sb_info *)fs_sb.s_fs_info)->s_es);
    ((struct ext4_sb_info *)fs_sb.s_fs_info)->s_es = NULL;
  }

  if (fs_sb.s_fs_info) {
    free((void *)fs_sb.s_fs_info);
    fs_sb.s_fs_info = NULL;
  }

  /*
   * No malloc here
   */
  fs_sb.s_type = NULL;
  fs_sb.s_op = NULL;
  fs_sb.s_d_op = NULL;

  memset((void *)&fs_sb, 0, sizeof(struct super_block));

  (void)io_close();

  return 0;
}

/*
 * Traverse dentry for child dentries
 */
static int32_t fs_traverse_dentry(struct dentry **dentry)
{
  struct super_block *sb = NULL;
  struct inode *inode = NULL;
  struct dentry *child = NULL;
  struct ext4_dir_entry_2 *ext4_dentries = NULL;
  uint32_t ext4_dentries_num, i;
  int32_t ret;

  if (!dentry || !*dentry) {
    return -1;
  }

  sb = (*dentry)->d_sb;
  inode = (*dentry)->d_inode;
  if (!sb || !inode) {
    return -1;
  }

  if (!(inode->i_mode & EXT4_INODE_MODE_S_IFDIR)) {
    return 0;
  }

  /*
   * Fill in Ext4 dentry
   */
  ext4_dentries_num = 0;
  if (ext4_raw_dentry_num(*dentry, &ext4_dentries_num) != 0) {
    ret = -1;
    goto fs_traverse_dentry_fail;
  }

  ext4_dentries = (struct ext4_dir_entry_2 *)malloc(ext4_dentries_num * sizeof(struct ext4_dir_entry_2));
  if (!ext4_dentries) {
    ret = -1;
    goto fs_traverse_dentry_fail;
  }
  memset((void *)ext4_dentries, 0, ext4_dentries_num * sizeof(struct ext4_dir_entry_2));

  if (ext4_raw_dentry(*dentry, ext4_dentries, ext4_dentries_num) != 0) {
    ret = -1;
    goto fs_traverse_dentry_fail;
  }

  /*
   * Allocate & instantiate child inodes & dentries
   */
  for (i = 0; i < ext4_dentries_num; ++i) {
    child = fs_create_child(sb, *dentry, (uint64_t)ext4_dentries[i].inode, (const unsigned char *)ext4_dentries[i].name, ext4_dentries[i].name_len);
    if (!child) {
      ret = -1;
      goto fs_traverse_dentry_fail;
    }
  }

  ret = 0;
  goto fs_traverse_dentry_exit;

 fs_traverse_dentry_fail:

  if (*dentry) {
    sb->s_d_op->d_release(*dentry);
    *dentry = NULL;
  }

  fs_destroy_inodes(sb);

 fs_traverse_dentry_exit:

  if (ext4_dentries) {
    free((void *)ext4_dentries);
    ext4_dentries = NULL;
  }

  return ret;
}

/*
 * Show stats of filesystem
 */
static int32_t fs_statfs(struct dentry *dentry, struct kstatfs *buf)
{
  if (!dentry || !buf) {
    return -1;
  }

  // TODO

  return 0;
}

/*
 * Show raw stats of filesystem
 */
static int32_t fs_statrawfs(struct dentry *dentry, const char **buf)
{
  struct ext4_super_block ext4_sb;
  int32_t ret;

  if (!dentry || !buf) {
    return -1;
  }

  /*
   * Fill in Ext4 superblock
   */
  memset((void *)&ext4_sb, 0, sizeof(struct ext4_super_block));
  ret = ext4_raw_super(&ext4_sb);
  if (ret != 0) {
    return -1;
  }

  memset((void *)fs_stat_sb, 0, sizeof(fs_stat_sb));
  ext4_show_stat_sb(&ext4_sb, fs_stat_sb, sizeof(fs_stat_sb));

  *buf = (const char *)fs_stat_sb;

  return 0;
}

/*
 * Show raw stats of file
 */
static int32_t fs_statraw(struct inode *inode, const char **buf)
{
  struct super_block *sb = NULL;
  struct ext4_sb_info *info = NULL;
  struct ext4_super_block *es = NULL;
  struct ext4_inode ext4_inode;
  uint64_t ino;
  int32_t ret;

  if (!inode || !buf) {
    return -1;
  }

  sb = inode->i_sb;
  info = (struct ext4_sb_info *)(sb->s_fs_info);
  es = info->s_es;
  ino = inode->i_ino;
  if (!sb || !info || !es) {
    return -1;
  }

  /*
   * Fill in Ext4 inode
   */
  ret = ext4_raw_inode(sb, ino, &ext4_inode);
  if (ret != 0) {
    return -1;
  }

  memset((void *)fs_stat_inode, 0, sizeof(fs_stat_inode));
  ext4_show_stat_inode(es, ino, &ext4_inode, fs_stat_inode, sizeof(fs_stat_inode));

  *buf = (const char *)fs_stat_inode;

  return 0;
}

/*
 * Open file for inode
 */
static int32_t fs_open(struct inode *inode, struct file *file)
{
  if (!inode || !file) {
    return -1;
  }

  memset((void *)&file->f_path, 0, sizeof(struct path));
  file->f_inode = inode;
  file->f_op = inode->i_fop;
  file->f_flags = 0;
  file->f_mode = 0;
  file->f_pos = 0;
  file->f_version = 0;

  return 0;
}

/*
 * Rlease file for inode
 */
static int32_t fs_release(struct inode *inode, struct file *file)
{
  if (!inode || !file) {
    return -1;
  }

  memset((void *)file, 0, sizeof(struct file));

  return 0;
}

/*
 * Read file at offset for inode
 */
static ssize_t fs_readat(struct file *file, int64_t offset, char *buf, size_t count, int64_t *num)
{
  if (!file || offset < 0 || !buf || count == 0 || !num) {
    return -1;
  }

  // TODO

  return 0;
}

/*
 * Init filesystem type
 */
struct file_system_type* fs_file_system_type_init_ext4(const char *type, int32_t flags)
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
