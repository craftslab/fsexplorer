/**
 * fs.h - The header of fs.
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

#ifndef _FS_H
#define _FS_H

#include "config.h"

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

#ifdef DEBUG
// Add code here
#endif

/*
 * Macro Definition
 */
#define FS_OPT_CMD_MOUNT   "mount"
#define FS_OPT_CMD_UMOUNT  "umount"
#define FS_OPT_CMD_STATS   "stats"
#define FS_OPT_CMD_STAT    "stat"
#define FS_OPT_CMD_PWD     "pwd"
#define FS_OPT_CMD_CD      "cd"
#define FS_OPT_CMD_LS      "ls"
#define FS_OPT_CMD_MKDIR   "mkdir"
#define FS_OPT_CMD_RM      "rm"
#define FS_OPT_CMD_CAT     "cat"
#define FS_OPT_CMD_ECHO    "echo"

#define FS_CURRENT_PATH  "."
#define FS_UPPER_PATH    ".."
#define FS_ROOT_PATH  "/"
#define FS_PATH_DELIM  "/"

/*
 * Type Definition
 */
typedef int32_t (*fs_opt_handle_t)(int32_t argc, const char **argv);

typedef struct {
  const char *opt_cmd;
  fs_opt_handle_t opt_hdl;
} fs_opt_t;

typedef struct {
  fs_opt_t opt_mount;
  fs_opt_t opt_umount;
  fs_opt_t opt_stats;
  fs_opt_t opt_stat;
  fs_opt_t opt_pwd;
  fs_opt_t opt_cd;
  fs_opt_t opt_ls;
  fs_opt_t opt_mkdir;
  fs_opt_t opt_rm;
  fs_opt_t opt_cat;
  fs_opt_t opt_echo;
} fs_opt_tbl_t;

#define FS_OPT_TBL_LEN  (sizeof(fs_opt_tbl_t) / sizeof(fs_opt_t))
#define FS_OPT_TBL_OFFSET  (sizeof(fs_opt_t))

/*
 * Function Declaration
 */

#endif /* _FS_H */
