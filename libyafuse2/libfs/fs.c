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
static void fs_unload_lib(void *handle);

static int32_t fs_mount(const char *devname, const char *dirname, const char *type, int32_t flags, void *data);
static int32_t fs_umount(const char *dirname, int32_t flags);
static int32_t fs_statfs(const char *pathname, struct fs_kstatfs *buf);
static int32_t fs_stat(const char *filename, struct fs_kstat *statbuf);
static int32_t fs_getdents(uint32_t fd, struct fs_dirent *dirent, uint32_t count);
static int32_t fs_getcwd(char *buf, uint64_t size);
static int32_t fs_chdir(const char *filename);

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
static void fs_unload_lib(void *handle)
{
#ifdef CMAKE_COMPILER_IS_GNUCC
  (void)dlclose(handle);
#else
  (void)FreeLibrary((HMODULE)handle);
#endif /* CMAKE_COMPILER_IS_GNUCC */
}

/*
 * Mount filesystem
 */
static int32_t fs_mount(const char *devname, const char *dirname, const char *type, int32_t flags, void *data)
{
  char lib_name[FS_LIB_NAME_LEN_MAX] = {0};
  fs_file_system_type_init_t handle = NULL;
  struct dentry *root = NULL;

  if (!devname || !dirname || !type || fs_mnt.mnt_count != 0) {
    return -1;
  }

#ifdef CMAKE_COMPILER_IS_GNUCC
  (void)snprintf(lib_name, FS_LIB_NAME_LEN_MAX, "lib%s.so", type);
#else
  (void)_snprintf(lib_name, FS_LIB_NAME_LEN_MAX, "%s.dll", type);
#endif /* CMAKE_COMPILER_IS_GNUCC */

  fs_lib_handle = fs_load_lib(lib_name);
  if (!fs_lib_handle) {
    goto fs_mount_helper_exit;
  }

  *(void **)(&handle) = fs_get_sym(fs_lib_handle, "fs_file_system_type_init");
  if (!handle) {
    goto fs_mount_helper_exit;
  }

  fs_type = handle(type, flags);
  if (!fs_type) {
    goto fs_mount_helper_exit;
  }

  root = fs_type->mount(fs_type, flags, devname, NULL);
  if (!root) {
    goto fs_mount_helper_exit;
  }

  memset((void *)&fs_mnt, 0, sizeof(struct mount));
  fs_mnt.mnt.mnt_root = root;
  fs_mnt.mnt.mnt_sb = root->d_sb;
  fs_mnt.mnt.mnt_flags = flags;
  fs_mnt.mnt_mountpoint = fs_mnt.mnt.mnt_root;
  fs_mnt.mnt_count = 1;
  fs_mnt.mnt_devname = devname;

  return 0;

fs_mount_helper_exit:

  fs_type = NULL;

  if (fs_lib_handle) {
    fs_unload_lib(fs_lib_handle);
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

  if (fs_type) {
    (void)fs_type->umount(dirname, flags);
    fs_type = NULL;
  }

  if (fs_lib_handle) {
    fs_unload_lib(fs_lib_handle);
    fs_lib_handle = NULL;
  }

  memset((void *)&fs_mnt, 0, sizeof(struct mount));

  return 0;
}

/*
 * Show stats of filesytem
 */
static int32_t fs_statfs(const char *pathname, struct fs_kstatfs *buf)
{
  if (!pathname || !buf) {
    return -1;
  }

  return 0;
}

/*
 * Show status of file
 */
static int32_t fs_stat(const char *filename, struct fs_kstat *statbuf)
{
  if (!filename || !statbuf) {
    return -1;
  }

  return 0;
}

/*
 * Get directory entries of filesystem
 */
static int32_t fs_getdents(uint32_t fd, struct fs_dirent *dirent, uint32_t count)
{
  if (!dirent) {
    return -1;
  }

  return 0;
}

/*
 * Get current working directory
 */
static int32_t fs_getcwd(char *buf, uint64_t size)
{
  if (!buf) {
    return -1;
  }

  return 0;
}

/*
 * Change working directory
 */
static int32_t fs_chdir(const char *filename)
{
  if (!filename) {
    return -1;
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

  (*fs_opt).mount = fs_mount;
  (*fs_opt).umount = fs_umount;
  (*fs_opt).statfs = fs_statfs;
  (*fs_opt).stat = fs_stat;
  (*fs_opt).getdents = fs_getdents;
  (*fs_opt).getcwd = fs_getcwd;
  (*fs_opt).chdir = fs_chdir;

  return 0;
}
