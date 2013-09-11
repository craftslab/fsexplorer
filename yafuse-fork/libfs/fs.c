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
#define FS_OPT_TBL_INIT_SYM  "fs_opt_tbl_init"

/*
 * Type Definition
 */

/*
 * Global Variable Definition
 */
static void *fs_lib_handle;
static fs_opt_tbl_t *fs_opt_tbl;
static int32_t fs_already_mounted;

/*
 * Function Declaration
 */
#ifdef CMAKE_COMPILER_IS_GNUCC
static void* fs_load_lib(const char *lib_name);
static void* fs_get_sym(void *handle, const char *symbol);
static void fs_unload_lib(void *handle);
#else
#endif /* CMAKE_COMPILER_IS_GNUCC */

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
int32_t fs_mount(const char *fs_name)
{
  int32_t argc = 0;
  const char* argv[FS_OPT_CMD_ARG_NUM_MAX] = {NULL};
  const char* lib_name = NULL;
  fs_opt_tbl_t* (*fs_opt_tbl_init)(void) = NULL;
  char *error = NULL;
  int32_t ret;

#ifdef CMAKE_COMPILER_IS_GNUCC
  lib_name = "libext4.so";
#else
  lib_name = "ext4.dll";
#endif /* CMAKE_COMPILER_IS_GNUCC */

  fs_lib_handle = fs_load_lib(lib_name);
  if (!fs_load_lib) {
    fs_already_mounted = 0;
    return -1;
  }

  dlerror();

  *(void **)(&fs_opt_tbl_init) = fs_get_sym(fs_lib_handle, FS_OPT_TBL_INIT_SYM);
  if ((error = dlerror()) != NULL) {
    ret = -1;
    goto fs_mount_err;
  }

  fs_opt_tbl = (fs_opt_tbl_t *)fs_opt_tbl_init();
  if (!fs_opt_tbl) {
    ret = -1;
    goto fs_mount_err;
  }

  if (!(fs_opt_tbl->opt_mount.opt_hdl)) {
    ret = -1;
    goto fs_mount_err;
  }

  argc = 2;
  argv[0] = FS_OPT_CMD_MOUNT;
  argv[1] = fs_name;

  ret = fs_opt_tbl->opt_mount.opt_hdl(argc, argv);
  if (ret != 0) {
    ret = -1;
    goto fs_mount_err;
  }

  fs_already_mounted = 1;

  return 0;

 fs_mount_err:

  fs_already_mounted = 0;

  fs_unload_lib(fs_lib_handle);

  return ret;
}

/*
 * Umount filesystem
 */
void fs_umount(void)
{
  int32_t argc = 0;
  const char* argv[FS_OPT_CMD_ARG_NUM_MAX] = {NULL};

  if (!fs_opt_tbl && !(fs_opt_tbl->opt_umount.opt_hdl)) {
    argc = 2;
    argv[0] = FS_OPT_CMD_UMOUNT;
    argv[1] = (const char *)NULL;

    (void)fs_opt_tbl->opt_umount.opt_hdl(argc, argv);

    fs_already_mounted = 0;
  }

  if (!fs_lib_handle) {
    fs_unload_lib(fs_lib_handle);
  }
}

/*
 * Get status of mount/umount
 */
int32_t fs_mounted(void)
{
  return fs_already_mounted;
}

/*
 * Match opt handle with opt command
 */
fs_opt_handle_t fs_opt_hdl_match(const char *fs_cmd)
{
  int32_t i = 0;
  size_t len_opt_cmd = 0, len_fs_cmd = 0;
  fs_opt_tbl_t *tbl = NULL;
  fs_opt_handle_t hdl = NULL;

  if (!fs_cmd) {
    return ((fs_opt_handle_t)NULL);
  }

  tbl = fs_opt_tbl;
  len_fs_cmd = strlen(fs_cmd);

  for (i = 0; i < FS_OPT_TBL_LEN; i += FS_OPT_TBL_OFFSET) {
    if (!tbl[i].opt_cmd || !tbl[i].opt_hdl) {
      continue;
    }
     
    len_opt_cmd = strlen((const char *)(tbl[i].opt_cmd));

    if (len_opt_cmd > 0 && len_opt_cmd <= len_fs_cmd) {
      if (strncmp((const char *)(tbl[i].opt_cmd), (const char *)fs_cmd, len_opt_cmd) == 0) {
        hdl = tbl[i].opt_hdl;
        break;
      }
    }
  }

  return hdl;
}

/*
 * Get number of filesystem opt
 */
int32_t fs_opt_num(void)
{
  return FS_OPT_TBL_LEN;
}

/*
 * Enumerate filesystem opt commond
 */
const char* fs_opt_cmd_enum(int32_t opt_idx)
{
  fs_opt_tbl_t *tbl = NULL;

  if (opt_idx < 0 || opt_idx >= FS_OPT_TBL_LEN) {
    return ((const char *)NULL);
  }

  tbl = fs_opt_tbl + (opt_idx * FS_OPT_TBL_OFFSET);
  if (!tbl->opt_hdl) {
    return NULL;

  return tbl->opt_cmd;
}
