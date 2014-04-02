/**
 * main.c - Main entry of test for libfs.
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
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <locale.h>
#include <limits.h>
#include <stdint.h>

#ifdef CMAKE_COMPILER_IS_GNUCC
#include <dlfcn.h>  
#else
#include <Windows.h>
#endif /* CMAKE_COMPILER_IS_GNUCC */

#ifdef DEBUG
// Add code here
#endif

#include "include/base/debug.h"
#include "include/libfs/libfs.h"

/*
 * Macro Definition
 */
#ifdef CMAKE_COMPILER_IS_GNUCC
#define LIB_NAME "libyafuse2.so"
#else
#define LIB_NAME "libyafuse2.dll"
#endif /* CMAKE_COMPILER_IS_GNUCC */

/*
 * Type Definition
 */

/*
 * Global Variable Definition
 */
static void *lib_handle;
static fs_opt_init_t opt_handle;

/*
 * Function Declaration
 */
static void show_banner(void);
static void show_usage(void);
static void* load_lib(const char *lib_name);
static void* get_sym(void *handle, const char *symbol);
static void unload_lib(void *handle);
static void show_stat(struct fs_kstat *stat);

/*
 * Function Definition
 */
static void show_banner(void)
{
  fprintf(stdout, "  __      _                     _           \n");
  fprintf(stdout, " / _|_ __(_) ___ __ _ _ __   __| | ___      \n");
  fprintf(stdout, "| |_| '__| |/ __/ _` | '_ \\ / _` |/ _ \\   \n");
  fprintf(stdout, "|  _| |  | | (_| (_| | | | | (_| | (_) |    \n");
  fprintf(stdout, "|_| |_|  |_|\\___\\__,_|_| |_|\\__,_|\\___/ \n");
  fprintf(stdout, "\n");
}

static void show_usage(void)
{
  fprintf(stdout, "\n");
  fprintf(stdout, "Usage:  fs-test <type> <image> <directory>\n");
  fprintf(stdout, "\n");
  fprintf(stdout, "Detail:\n");
  fprintf(stdout, "  <type>       fs type. e.g., ext4 or fat\n");
  fprintf(stdout, "  <image>      fs image, e.g., fs-image.ext4,\n");
  fprintf(stdout, "               or fs-image.fat\n");
  fprintf(stdout, "  <directory>  fs mount point\n");
  fprintf(stdout, "\n");
  fprintf(stdout, "Example:\n");
  fprintf(stdout, "  fs-test ext4 fs-image.ext4 fs-mnt\n");
  fprintf(stdout, "\n");
}

static void* load_lib(const char *lib_name)
{
#ifdef CMAKE_COMPILER_IS_GNUCC
  return dlopen(lib_name, RTLD_LAZY);
#else
  return (void *)LoadLibrary(lib_name);
#endif /* CMAKE_COMPILER_IS_GNUCC */
}

static void* get_sym(void *handle, const char *symbol)
{
#ifdef CMAKE_COMPILER_IS_GNUCC
  void *sym = NULL;
  const char *error = NULL;

  dlerror();

  *(void **)(&sym) = dlsym(handle, symbol);

  error = dlerror();
  if (error) {
    return NULL;
  }

  return sym;
#else
  return (void *)GetProcAddress((HMODULE)handle, (LPCSTR)symbol);
#endif /* CMAKE_COMPILER_IS_GNUCC */
}

static void unload_lib(void *handle)
{
#ifdef CMAKE_COMPILER_IS_GNUCC
  (void)dlclose(handle);
#else
  (void)FreeLibrary((HMODULE)handle);
#endif /* CMAKE_COMPILER_IS_GNUCC */
}

static void show_stat(struct fs_kstat *stat)
{
  info("stat of inode %llu", (long long unsigned)stat->ino);
  info("ino: %llu", (long long unsigned)stat->ino);
  info("mode: 0x%x", stat->mode);
  info("nlink: %d", stat->nlink);
  info("uid: %d", stat->uid);
  info("gid: %d", stat->gid);
  info("size: %lld", (long long int)stat->size);
  info("atime: sec %lld nsec %lld", (long long int)stat->atime.tv_sec, (long long int)stat->atime.tv_nsec);
  info("mtime: sec %lld nsec %lld", (long long int)stat->mtime.tv_sec, (long long int)stat->mtime.tv_nsec);
  info("ctime: sec %lld nsec %lld", (long long int)stat->ctime.tv_sec, (long long int)stat->ctime.tv_nsec);
  info("blksize: %llu", (long long unsigned)stat->blksize);
  info("blocks: %llu\n", (long long unsigned)stat->blocks);
}

int32_t main(int argc, char *argv[])
{
  const char *fs_type = NULL, *fs_img = NULL, *fs_mnt = NULL;
  struct fs_opt_t fs_opt;
  struct fs_dirent fs_root;
  struct fs_kstatfs fs_statfs;
  struct fs_kstat fs_stat;
  struct fs_dirent fs_dirent;
  struct fs_dirent *fs_dirents = NULL;
  uint32_t fs_dirents_num;
  uint64_t ino;
  uint32_t i;
  int32_t ret = 0;

  show_banner();

  if (argc != 4) {
    show_usage();
    return 0;
  }

  fs_type = argv[1];
  fs_img = argv[2];
  fs_mnt = argv[3];
  memset((void *)&fs_opt, 0, sizeof(struct fs_opt_t));

  /*
   * Load fs library
   */
  lib_handle = load_lib(LIB_NAME);
  if (!lib_handle) {
    error("load_lib failed!");
    ret = -1;
    goto main_exit;
  }

  *(void **)(&opt_handle) = get_sym(lib_handle, FS_LIB_SYMBOL);
  if (!opt_handle) {
    error("get_sym failed!");
    ret = -1;
    goto main_exit;
  }

  ret = opt_handle(&fs_opt);
  if (ret != 0) {
    error("opt_handle failed!");
    goto main_exit;
  }

  /*
   * Mount fs image
   */
  memset((void *)&fs_root, 0, sizeof(struct fs_dirent));
  ret = fs_opt.mount(fs_img, fs_mnt, fs_type, 0, &fs_root);
  if (ret != 0) {
    error("mount failed!");
    goto main_exit;
  }
  info("mount filesystem successfully.\n");

  info("root dentry");
  info("ino %llu", (long long unsigned)fs_root.d_ino);
  info("type %d", fs_root.d_type);
  info("name %s\n", fs_root.d_name);

  /*
   * Show stats of fs
   */
  memset((void *)&fs_statfs, 0, sizeof(struct fs_kstatfs));
  ret = fs_opt.statfs(fs_mnt, &fs_statfs);
  if (ret != 0) {
    error("statfs failed!");
    goto main_exit;
  }

  /*
   * Show stats of file
   */
  memset((void *)&fs_stat, 0, sizeof(struct fs_kstat));
  ret = fs_opt.stat(fs_root.d_ino, &fs_stat);
  if (ret != 0) {
    error("stat failed!");
    goto main_exit;
  }
  show_stat(&fs_stat);

  /*
   * Get dentry list
   */
  ino = fs_root.d_ino;

  ret = fs_opt.querydent(ino, &fs_dirent);
  if (ret != 0) {
    error("querydent failed!");
    goto main_exit;
  }

  fs_dirents_num = fs_dirent.d_childnum;
  fs_dirents = (struct fs_dirent *)malloc(sizeof(struct fs_dirent) * fs_dirents_num);
  if (!fs_dirents) {
    error("malloc failed!");
    goto main_exit;
  }
  memset((void *)fs_dirents, 0, sizeof(struct fs_dirent) * fs_dirents_num);

  ret = fs_opt.getdents(ino, fs_dirents, fs_dirents_num);
  if (ret != 0) {
    error("getdents failed!");
    goto main_exit;
  }

  info("child dentries of inode %llu", (long long unsigned)ino);
  for (i = 0; i < fs_dirents_num; ++i) {
    info("name %s ino %llu type %d", fs_dirents[i].d_name, (long long unsigned)fs_dirents[i].d_ino, fs_dirents[i].d_type);
  }
  fprintf(stdout, "\n");

  ret = 0;

main_exit:

  if (fs_dirents) {
    free((void *)fs_dirents);
    fs_dirents = NULL;
  }

  /*
   * Umount fs image
   */
  if (fs_opt.umount) {
    (void)fs_opt.umount(fs_mnt, 0);
    info("unmount filesystem successfully.");
  }

  if (lib_handle) unload_lib(lib_handle);

  return ret;
}
