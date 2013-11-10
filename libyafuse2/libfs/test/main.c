/**
 * main.c - Main entry of test for libfs.
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
#define LIB_NAME "libfs.so"
#else
#define LIB_NAME "fs.dll"
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
  fprintf(stdout, "  fs-test ext4 fs-image.ext4 fs-mount\n");
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
  return dlsym(handle, symbol);
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

int32_t main(int argc, char *argv[])
{
  const char *fs_type = NULL, *fs_img = NULL, *fs_mnt = NULL;
  struct fs_opt_t fs_opt;
  int32_t ret = 0;

  show_banner();

  if (argc != 4) {
    show_usage();
    return 0;
  }

  fs_type = argv[1];
  fs_img = argv[2];
  fs_mnt = argv[3];

  lib_handle = load_lib(LIB_NAME);
  if (!lib_handle) {
    fprintf(stderr, "error: load_lib failed!\n");
    ret = -1;
    goto main_exit;
  }

  *(void **)(&opt_handle) = get_sym(lib_handle, "fs_opt_init");
  if (!opt_handle) {
    fprintf(stderr, "error: get_sym failed!\n");
    ret = -1;
    goto main_exit;
  }

  ret = opt_handle(&fs_opt);
  if (ret != 0) {
    fprintf(stderr, "error: opt_handle failed!\n");
    goto main_exit;
  }

  ret = fs_opt.mount(fs_img, fs_mnt, fs_type, 0, NULL);
  if (ret != 0) {
    fprintf(stderr, "error: mount failed!\n");
    goto main_exit;
  }

  ret = fs_opt.umount("mount-point", 0);
  if (ret != 0) {
    fprintf(stderr, "error: umount failed!\n");
    goto main_exit;
  }

  ret = 0;

 main_exit:

  if (lib_handle) unload_lib(lib_handle);

  return ret;
}
