/**
 * fs.c - The entry of filesystem.
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
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef CMAKE_COMPILER_IS_GNUCC
#include <dlfcn.h>  
#else
#include <Windows.h>
#endif /* CMAKE_COMPILER_IS_GNUCC */

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

/*
 * Global Variable Definition
 */
static void *fs_lib_handle = NULL;
static struct file_system_type *fs_type = NULL;
static struct mount fs_mnt;

/*
 * Function Declaration
 */
static void* fs_load_lib(const char *libname);
static void* fs_get_sym(void *handle, const char *symbol);
static int32_t fs_unload_lib(void *handle);
static int32_t fs_get_inode(struct super_block *sb, uint64_t ino, struct inode *inode);
static int32_t fs_get_dentry(struct dentry *root, uint64_t ino, struct dentry *dentry);
static int32_t fs_stat_helper(struct super_block *sb, struct inode *inode, struct fs_kstat *stat);

static int32_t fs_mount(const char *devname, const char *dirname, const char *type, int32_t flags, struct fs_dirent *dirent);
static int32_t fs_umount(const char *dirname, int32_t flags);
static int32_t fs_statfs(const char *pathname, struct fs_kstatfs *buf);
static int32_t fs_stat(uint64_t ino, struct fs_kstat *buf);
static int32_t fs_getdents(uint64_t ino, struct fs_dirent *dirent, uint32_t count);

/*
 * Function Definition
 */
/*
 * Load library
 */
static void* fs_load_lib(const char *libname)
{
#ifdef CMAKE_COMPILER_IS_GNUCC
  return dlopen(libname, RTLD_LAZY);
#else
  return (void *)LoadLibrary(libname);
#endif /* CMAKE_COMPILER_IS_GNUCC */
}

/*
 * Get symbol in library
 */
static void* fs_get_sym(void *handle, const char *symbol)
{
#ifdef CMAKE_COMPILER_IS_GNUCC
  return dlsym(handle, symbol);
#else
  return (void *)GetProcAddress((HMODULE)handle, (LPCSTR)symbol);
#endif /* CMAKE_COMPILER_IS_GNUCC */
}

/*
 * Unload library
 */
static int32_t fs_unload_lib(void *handle)
{
#ifdef CMAKE_COMPILER_IS_GNUCC
  (void)dlclose(handle);
#else
  (void)FreeLibrary((HMODULE)handle);
#endif /* CMAKE_COMPILER_IS_GNUCC */

  return 0;
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

#ifdef CMAKE_COMPILER_IS_GNUCC
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
 * Get dentry from ino of filesystem
 */
static int32_t fs_get_dentry(struct dentry *root, uint64_t ino, struct dentry *dentry)
{
  struct dentry *child = NULL;
  int32_t ret = -1;

  if (root->d_inode->i_ino == ino) {
    *dentry = *root;
    return 0;
  }

  if (!list_empty(&root->d_subdirs)) {
#ifdef CMAKE_COMPILER_IS_GNUCC
    list_for_each_entry(child, &root->d_subdirs, d_child) {
#else
    for (child = list_entry((&root->d_subdirs)->next, struct dentry, d_child);
         &child->d_child != (&root->d_subdirs);
         child = list_entry(child->d_child.next, struct dentry, d_child)) {
#endif
      ret = fs_get_dentry(child, ino, dentry);
      if (ret == 0) {
        break;
      }
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
  char lib_name[FS_LIB_NAME_LEN_MAX] = {0};
  fs_file_system_type_init_t handle = NULL;
  struct dentry *root = NULL;
  int32_t len;

  if (!devname || !dirname || !type || !dirent || fs_mnt.mnt_count != 0) {
    return -1;
  }

#ifdef CMAKE_COMPILER_IS_GNUCC
  (void)snprintf(lib_name, FS_LIB_NAME_LEN_MAX - 1, "lib%s.so", type);
#else
  (void)_snprintf(lib_name, FS_LIB_NAME_LEN_MAX - 1, "lib%s.dll", type);
#endif /* CMAKE_COMPILER_IS_GNUCC */

  fs_lib_handle = fs_load_lib(lib_name);
  if (!fs_lib_handle) {
    goto fs_mount_exit;
  }

  *(void **)(&handle) = fs_get_sym(fs_lib_handle, "fs_file_system_type_init");
  if (!handle) {
    goto fs_mount_exit;
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
  dirent->d_ino = (uint64_t)fs_mnt.mnt_mountpoint->d_inode->i_ino;
  dirent->d_off = (int64_t)-1;
  dirent->d_reclen = (uint16_t)0;
  dirent->d_type = (enum libfs_ftype)FT_DIR;
  len = (int32_t)fs_mnt.mnt_mountpoint->d_name->len;
  len = len >= 255 ? 255 : len;
  memcpy((void *)dirent->d_name, fs_mnt.mnt_mountpoint->d_name->name, len);

  return 0;

fs_mount_exit:

  fs_type = NULL;

  if (fs_lib_handle) {
    (void)fs_unload_lib(fs_lib_handle);
    fs_lib_handle = NULL;
  }

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

  if (fs_lib_handle) {
    (void)fs_unload_lib(fs_lib_handle);
    fs_lib_handle = NULL;
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
 * Get directory entries of filesystem
 */
static int32_t fs_getdents(uint64_t ino, struct fs_dirent *dirent, uint32_t count)
{
  struct dentry *root = fs_mnt.mnt.mnt_root;
  struct dentry dentry;
  int32_t ret;

  if (!dirent || count == 0) {
    return -1;
  }

  memset((void *)&dentry, 0, sizeof(struct dentry));
  ret = fs_get_dentry(root, ino, &dentry);
  if (ret != 0) {
    return -1;
  }

  // add code here

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

  (*fs_opt).mount = fs_mount;
  (*fs_opt).umount = fs_umount;
  (*fs_opt).statfs = fs_statfs;
  (*fs_opt).stat = fs_stat;
  (*fs_opt).getdents = fs_getdents;

  return 0;
}
