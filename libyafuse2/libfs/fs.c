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

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

#ifdef CMAKE_COMPILER_IS_GNUCC
#include <dlfcn.h>  
#else
  // Add code here
#endif /* CMAKE_COMPILER_IS_GNUCC */

#ifdef DEBUG
// Add code here
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
static void *fs_lib_handle;
static int32_t fs_already_mounted;

/*
 * Function Declaration
 */
static void* fs_load_lib(const char *lib_name);
static void* fs_get_sym(void *handle, const char *symbol);
static void fs_unload_lib(void *handle);
static int32_t fs_mount(const char *dev_name, const char *dir_name, const char *type, uint32_t flags, void *data);
static int32_t fs_umount(const char *name, int32_t flags);

/*
 * Function Definition
 */
/*
 * Load library
 */
static void* fs_load_lib(const char *lib_name)
{
#ifdef CMAKE_COMPILER_IS_GNUCC
  return dlopen(lib_name, RTLD_LAZY);
#else
  return (void *)LoadLibrary(LPCTSTR(lib_name));
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
static int32_t fs_mount(const char *dev_name, const char *dir_name, const char *type, uint32_t flags, void *data)
{
  char lib_name[FS_LIB_NAME_LEN_MAX] = {0};

  if (!dev_name || !dir_name || !type) {
    fs_already_mounted = 0;
    return -1;
  }

#ifdef CMAKE_COMPILER_IS_GNUCC
  (void)snprintf(lib_name, FS_LIB_NAME_LEN_MAX, "lib%s.so", type);
#else
  (void)snprintf(lib_name, FS_LIB_NAME_LEN_MAX, "%s.dll", type);
#endif /* CMAKE_COMPILER_IS_GNUCC */

  fs_lib_handle = fs_load_lib(lib_name);
  if (!fs_lib_handle) {
    goto fs_mount_exit;
  }

  fs_get_sym(fs_lib_handle, "dummy");

  fs_already_mounted = 1;

  return 0;

 fs_mount_exit:

  fs_already_mounted = 0;
  if (fs_lib_handle) fs_unload_lib(fs_lib_handle);

  return -1;
}

/*
 * Unmount filesystem
 */
static int32_t fs_umount(const char *name, int32_t flags)
{
  fs_already_mounted = 0;
  if (fs_lib_handle) fs_unload_lib(fs_lib_handle);
  return 0;
}

/*
 * Init filesystem operation
 */
int32_t fs_opt_init(struct fs_opt_t *fs_opt)
{
  if (!fs_opt) {
    return -1;
  }

  (*fs_opt).mount = fs_mount;
  (*fs_opt).umount = fs_umount;

  return 0;
}
