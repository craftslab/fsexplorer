/**
 * fs.c - The entry of filesystem.
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

#include "config.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef DEBUG
#define DEBUG_LIBFS_FS
#endif

#include "include/base/debug.h"
#include "include/fs.h"
#include "include/libfs/libfs.h"

/*
 * Macro Definition
 */
#define FS_LIB_NAME_LEN_MAX 16

/*
 * Type Definition
 */
struct fs_type_list_t {
  const char *type;
  fs_file_system_type_init_t handle;
};

/*
 * Global Variable Definition
 */
static struct fs_type_list_t fs_type_list[] = {
  {
    FS_TYPE_EXT4,
    fs_file_system_type_init_ext4,
  },

  {
    NULL,
    NULL,
  },
};

static struct file_system_type *fs_type = NULL;
static struct mount fs_mnt;

/*
 * Function Declaration
 */
static int32_t fs_imode2ftype(enum libfs_imode imode, enum libfs_ftype *ftype);
static int32_t fs_dentry2dirent(struct dentry *dentry, struct fs_dirent *dirent);
static int32_t fs_traverse_dentry(struct dentry **dentry);
static int32_t fs_get_dentry(struct dentry *dentry, uint64_t ino, struct dentry **match);
static int32_t fs_get_inode(struct super_block *sb, uint64_t ino, struct inode *inode);
static int32_t fs_stat_helper(struct super_block *sb, struct inode *inode, struct fs_kstat *stat);

static int32_t fs_mount(const char *devname, const char *dirname, const char *type, int32_t flags, struct fs_dirent *dirent);
static int32_t fs_umount(const char *dirname, int32_t flags);
static int32_t fs_statfs(const char *pathname, struct fs_kstatfs *buf);
static int32_t fs_stat(uint64_t ino, struct fs_kstat *buf);
static int32_t fs_querydent(uint64_t ino, struct fs_dirent *dirent);
static int32_t fs_getdents(uint64_t ino, struct fs_dirent *dirents, uint32_t dirents_num);

/*
 * Function Definition
 */
/*
 * Get dirent from dentry
 */
static int32_t fs_imode2ftype(enum libfs_imode imode, enum libfs_ftype *ftype)
{
  if (imode & IFIFO) {
    *ftype = FT_FIFO;
  } else if (imode & IFCHR) {
    *ftype = FT_CHRDEV;
  } else if (imode & IFDIR) {
    *ftype = FT_DIR;
  } else if (imode & IFBLK) {
    *ftype = FT_BLKDEV;
  } else if (imode & IFREG) {
    *ftype = FT_REG_FILE;
  } else if (imode & IFLNK) {
    *ftype = FT_SYMLINK;
  } else if (imode & IFSOCK) {
    *ftype = FT_SOCK;
  } else {
    *ftype = FT_UNKNOWN;
  }

  return 0;
}

/*
 * Get dirent from dentry
 */
static int32_t fs_dentry2dirent(struct dentry *dentry, struct fs_dirent *dirent)
{
  int32_t len;
  int32_t ret;

  dirent->d_ino = (uint64_t)dentry->d_inode->i_ino;
  dirent->d_off = (int64_t)-1;
  dirent->d_reclen = (uint16_t)0;
  ret = fs_imode2ftype((enum libfs_imode)dentry->d_inode->i_mode, &dirent->d_type);
  if (ret != 0) {
    return -1;
  }

  len = (int32_t)dentry->d_name->len;
  len = len > FS_DNAME_LEN - 1 ? FS_DNAME_LEN - 1 : len;
  memset((void *)dirent->d_name, 0, FS_DNAME_LEN);
  memcpy((void *)dirent->d_name, (const void *)dentry->d_name->name, len);

  dirent->d_childnum = dentry->d_childnum;

  return 0;
}

/*
 * Traverse dentry for chlid dentries
 */
static int32_t fs_traverse_dentry(struct dentry **dentry)
{
  struct super_block *sb = fs_mnt.mnt.mnt_sb;
  int32_t ret;

  if (!sb || !sb->s_op || !sb->s_op->traverse_dentry) {
    return -1;
  }

  if (!list_empty(&(*dentry)->d_subdirs)) {
    return 0;
  }

  ret = sb->s_op->traverse_dentry(dentry);
  if (ret != 0) {
    return -1;
  }

  return 0;
}

/*
 * Get dentry from ino of filesystem
 */
static int32_t fs_get_dentry(struct dentry *dentry, uint64_t ino, struct dentry **match)
{
  struct dentry *child = NULL;
  int32_t ret = -1;

  if (dentry->d_inode->i_ino == ino) {
    *match = dentry;
    return 0;
  }

  if (!list_empty(&dentry->d_subdirs)) {
#if 0  // For CMAKE_COMPILER_IS_GNUCC only
    list_for_each_entry(child, &dentry->d_subdirs, d_child) {
#else
    for (child = list_entry((&dentry->d_subdirs)->next, struct dentry, d_child);
         &child->d_child != (&dentry->d_subdirs);
         child = list_entry(child->d_child.next, struct dentry, d_child)) {
#endif
      ret = fs_get_dentry(child, ino, match);
      if (ret == 0) {
        break;
      }
    }
  }

  return ret;
}

/*
 * Get inode from ino of filesystem
 */
static int32_t fs_get_inode(struct super_block *sb, uint64_t ino, struct inode *inode)
{
  struct inode *child = NULL;
  int32_t ret = -1;

  if (list_empty(&sb->s_inodes)) {
    return -1;
  }

#if 0  // For CMAKE_COMPILER_IS_GNUCC only
  list_for_each_entry(child, &sb->s_inodes, i_sb_list) {
#else
  for (child = list_entry((&sb->s_inodes)->next, struct inode, i_sb_list);
       &child->i_sb_list != (&sb->s_inodes);
       child = list_entry(child->i_sb_list.next, struct inode, i_sb_list)) {
#endif
    if (child->i_ino == ino) {
      *inode = *child;
      ret = 0;
      break;
    }
  }

  return ret;
}

/*
 * Fill in stats of file
 */
static int32_t fs_stat_helper(struct super_block *sb, struct inode *inode, struct fs_kstat *stat)
{
  if (sizeof(struct libfs_timespec) != sizeof(struct fs_timespec)) {
    return -1;
  }

  stat->ino = (uint64_t)inode->i_ino;
  stat->mode = (enum libfs_imode)inode->i_mode;
  stat->nlink = (uint32_t)inode->i_count;
  stat->uid = (uint32_t)inode->i_uid;
  stat->gid = (uint32_t)inode->i_gid;
  stat->size = (int64_t)inode->i_size;
  memcpy((void *)&stat->atime, (const void*)&inode->i_atime, sizeof(struct libfs_timespec));
  memcpy((void *)&stat->mtime, (const void*)&inode->i_mtime, sizeof(struct libfs_timespec));
  memcpy((void *)&stat->ctime, (const void*)&inode->i_ctime, sizeof(struct libfs_timespec));
  stat->blksize = (uint64_t)sb->s_blocksize;
  stat->blocks = (uint64_t)inode->i_blocks;

  return 0;
}

/*
 * Mount filesystem
 */
static int32_t fs_mount(const char *devname, const char *dirname, const char *type, int32_t flags, struct fs_dirent *dirent)
{
  fs_file_system_type_init_t handle = NULL;
  struct dentry *root = NULL;
  int32_t i, len;

  if (!devname || !dirname || !type || !dirent || fs_mnt.mnt_count != 0) {
    return -1;
  }

  len = sizeof(fs_type_list) / sizeof(fs_type_list[0]);

  for (i = 0; i < len; ++i) {
    if (!memcmp((const void *)type, (const void *)fs_type_list[i].type, strlen(fs_type_list[i].type))) {
      handle = fs_type_list[i].handle;
      break;
    }
  }

  if (!handle) {
    return -1;
  }

  fs_type = handle(type, flags);
  if (!fs_type || !fs_type->mount) {
    goto fs_mount_exit;
  }

  root = fs_type->mount(fs_type, flags, devname, NULL);
  if (!root) {
    goto fs_mount_exit;
  }

  memset((void *)&fs_mnt, 0, sizeof(struct mount));
  fs_mnt.mnt.mnt_root = root;
  fs_mnt.mnt.mnt_sb = root->d_sb;
  fs_mnt.mnt.mnt_flags = flags;
  fs_mnt.mnt_mountpoint = fs_mnt.mnt.mnt_root;
  fs_mnt.mnt_count = 1;
  fs_mnt.mnt_devname = devname;

  memset((void *)dirent, 0, sizeof(struct fs_dirent));
  if (fs_dentry2dirent(fs_mnt.mnt_mountpoint, dirent) != 0) {
    goto fs_mount_exit;
  }

  return 0;

fs_mount_exit:

  fs_type = NULL;

  return -1;
}

/*
 * Unmount filesystem
 */
static int32_t fs_umount(const char *dirname, int32_t flags)
{
  if (!dirname || fs_mnt.mnt_count == 0) {
    return -1;
  }

  if (fs_type && fs_type->umount) {
    (void)fs_type->umount(dirname, flags);
    fs_type = NULL;
  }

  memset((void *)&fs_mnt, 0, sizeof(struct mount));

  return 0;
}

/*
 * Show stats of filesystem
 */
static int32_t fs_statfs(const char *pathname, struct fs_kstatfs *buf)
{
  struct super_block *sb = fs_mnt.mnt.mnt_sb;
  struct dentry *root = fs_mnt.mnt.mnt_root;
  struct kstatfs rootbuf;
  int32_t ret;

  if (!pathname || !buf) {
    return -1;
  }

  if (!sb || !sb->s_op || !sb->s_op->statfs) {
    return -1;
  }

  memset((void *)&rootbuf, 0, sizeof(struct kstatfs));
  ret = sb->s_op->statfs(root, &rootbuf);
  if (ret != 0) {
    return -1;
  }

  buf->f_type = (enum libfs_ftype)rootbuf.f_type;
  buf->f_bsize = (int64_t)rootbuf.f_bsize;
  buf->f_blocks = (uint64_t)rootbuf.f_blocks;
  buf->f_bfree = (uint64_t)rootbuf.f_bfree;
  buf->f_bavail = (uint64_t)rootbuf.f_bavail;
  buf->f_files = (uint64_t)rootbuf.f_files;
  buf->f_ffree = (uint64_t)rootbuf.f_ffree;
  memcpy((void *)&buf->f_fsid, (const void *)&rootbuf.f_fsid, sizeof(struct fs_fsid_t));
  buf->f_namelen = (int64_t)rootbuf.f_namelen;
  buf->f_frsize = (int64_t)rootbuf.f_frsize;
  buf->f_flags = (int64_t)rootbuf.f_flags;
  memcpy((void *)buf->f_spare, (const void *)rootbuf.f_spare, sizeof(int64_t) * 4);

  return 0;
}

/*
 * Show stats of file
 */
static int32_t fs_stat(uint64_t ino, struct fs_kstat *buf)
{
  struct super_block *sb = fs_mnt.mnt.mnt_sb;
  struct inode inode;
  int32_t ret;

  if (!buf) {
    return -1;
  }

  if (!sb) {
    return -1;
  }

  memset((void *)&inode, 0, sizeof(struct inode));
  ret = fs_get_inode(sb, ino, &inode);
  if (ret != 0) {
    return -1;
  }

  (void)fs_stat_helper(sb, &inode, buf);

  return 0;
}

/*
 * Query dirent for ino
 */
static int32_t fs_querydent(uint64_t ino, struct fs_dirent *dirent)
{
  struct dentry *root = fs_mnt.mnt.mnt_root;
  struct dentry *parent = NULL;
  int32_t ret;

  if (!dirent) {
    return -1;
  }

  /*
   * Get parent dentry mached with ino
   */
  ret = fs_get_dentry(root, ino, &parent);
  if (ret != 0) {
    return -1;
  }

  /*
   * Check if parent dentry has child dentries and
   * if child dentries exist and not be traversed yet, traverse them now,
   * otherwise just ignore it
   */
  ret = fs_traverse_dentry(&parent);
  if (ret != 0) {
    return -1;
  }

  ret = fs_dentry2dirent(parent, dirent);
  if (ret != 0) {
    return -1;
  }

  return 0;
}

/*
 * Get directory entries of filesystem
 */
static int32_t fs_getdents(uint64_t ino, struct fs_dirent *dirents, uint32_t dirents_num)
{
  struct dentry *root = fs_mnt.mnt.mnt_root;
  struct dentry *parent = NULL, *child = NULL;
  uint32_t i;
  int32_t ret;

  if (!dirents || dirents_num == 0) {
    return -1;
  }

  /*
   * Get parent dentry mached with ino
   */
  ret = fs_get_dentry(root, ino, &parent);
  if (ret != 0) {
    return -1;
  }

  /*
   * Check if parent dentry has child dentries and
   * if child dentries exist and not be traversed yet, traverse them now,
   * otherwise just ignore it
   */
  ret = fs_traverse_dentry(&parent);
  if (ret != 0) {
    return -1;
  }

  /*
   * Populate child dentries
   */
  i = 0;

  if (!list_empty(&parent->d_subdirs)) {
#if 0  // For CMAKE_COMPILER_IS_GNUCC only
    list_for_each_entry_reverse(child, &parent->d_subdirs, d_child) {
#else
    for (child = list_entry((&parent->d_subdirs)->prev, struct dentry, d_child);
         &child->d_child != (&parent->d_subdirs);
         child = list_entry(child->d_child.prev, struct dentry, d_child)) {
#endif
      if (++i > dirents_num) {
        return -1;
      }

      ret = fs_dentry2dirent(child, &dirents[i - 1]);
      if (ret != 0) {
        return -1;
      }
    }
  }

  return 0;
}

/*
 * Init filesystem operation
 */
#ifdef CMAKE_COMPILER_IS_GNUCC
int32_t fs_opt_init(struct fs_opt_t *fs_opt)
#else
__declspec(dllexport) int32_t fs_opt_init(struct fs_opt_t *fs_opt)
#endif /* CMAKE_COMPILER_IS_GNUCC */
{
  if (!fs_opt) {
    return -1;
  }

  fs_opt->mount = fs_mount;
  fs_opt->umount = fs_umount;
  fs_opt->statfs = fs_statfs;
  fs_opt->stat = fs_stat;
  fs_opt->querydent = fs_querydent;
  fs_opt->getdents = fs_getdents;

  return 0;
}
