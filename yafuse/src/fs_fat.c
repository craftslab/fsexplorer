/**
 * fs_fat.c - The entry of FAT filesystem.
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
 * along with this program (in the main directory of the NTFS-3G
 * distribution in the file COPYING); if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_STDIO_H
#include <stdio.h>
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

#ifdef DEBUG
// Add code here
#endif

#include "include/debug.h"
#include "include/types.h"
#include "include/libfat/libfat.h"
#include "include/libio/io.h"

#include "filesystem.h"
#include "fs_fat.h"

/*
 * Macro Definition
 */

/*
 * Type Definition
 */

/*
 * Function Declaration
 */
static int32_t fs_do_mount(int32_t argc, const char **argv);
static int32_t fs_do_umount(int32_t argc, const char **argv);
static int32_t fs_do_stats(int32_t argc, const char **argv);
static int32_t fs_do_stat(int32_t argc, const char **argv);
static int32_t fs_do_pwd(int32_t argc, const char **argv);
static int32_t fs_do_cd(int32_t argc, const char **argv);
static int32_t fs_do_ls(int32_t argc, const char **argv);
static int32_t fs_do_mkdir(int32_t argc, const char **argv);
static int32_t fs_do_rm(int32_t argc, const char **argv);
static int32_t fs_do_read(int32_t argc, const char **argv);
static int32_t fs_do_write(int32_t argc, const char **argv);

/*
 * Global Variable Definition
 */
/*
 * FAT filesystem operation table
 */
fs_opt_t fs_opt_tbl_fat[FS_OPT_TBL_NUM_MAX] = {
  [0] = {
    .opt_hdl = fs_do_mount,
    .opt_cmd = FS_OPT_CMD_MOUNT,
  },

  [1] = {
    .opt_hdl = fs_do_umount,
    .opt_cmd = FS_OPT_CMD_UMOUNT,
  },

  [2] = {
    .opt_hdl = fs_do_stats,
    .opt_cmd = "stats",
  },

  [3] = {
    .opt_hdl = fs_do_stat,
    .opt_cmd = "stat",
  },

  [4] = {
    .opt_hdl = fs_do_pwd,
    .opt_cmd = "pwd",
  },

  [5] = {
    .opt_hdl = fs_do_cd,
    .opt_cmd = "cd",
  },

  [6] = {
    .opt_hdl = fs_do_ls,
    .opt_cmd = "ls",
  },

  [7] = {
    .opt_hdl = fs_do_mkdir,
    .opt_cmd = "mkdir",
  },

  [8] = {
    .opt_hdl = fs_do_rm,
    .opt_cmd = "rm",
  },

  [9] = {
    .opt_hdl = fs_do_read,
    .opt_cmd = "read",
  },

  [10] = {
    .opt_hdl = fs_do_write,
    .opt_cmd = "write",
  },

  [11] = {
    .opt_hdl = NULL,
    .opt_cmd = NULL,
  }
};

/*
 * Function Definition
 */
static int32_t fs_do_mount(int32_t argc, const char **argv)
{
  argc = argc;
  argv = argv;

  return -1;
}

static int32_t fs_do_umount(int32_t argc, const char **argv)
{
  argc = argc;
  argv = argv;

  return -1;
}

static int32_t fs_do_stats(int32_t argc, const char **argv)
{
  argc = argc;
  argv = argv;

  return -1;
}

static int32_t fs_do_stat(int32_t argc, const char **argv)
{
  argc = argc;
  argv = argv;

  return -1;
}

static int32_t fs_do_pwd(int32_t argc, const char **argv)
{
  argc = argc;
  argv = argv;

  return -1;
}

static int32_t fs_do_cd(int32_t argc, const char **argv)
{
  argc = argc;
  argv = argv;

  return -1;
}

static int32_t fs_do_ls(int32_t argc, const char **argv)
{
  argc = argc;
  argv = argv;

  return -1;
}

static int32_t fs_do_mkdir(int32_t argc, const char **argv)
{
  argc = argc;
  argv = argv;

  return -1;
}

static int32_t fs_do_rm(int32_t argc, const char **argv)
{
  argc = argc;
  argv = argv;

  return -1;
}

static int32_t fs_do_read(int32_t argc, const char **argv)
{
  argc = argc;
  argv = argv;

  return -1;
}

static int32_t fs_do_write(int32_t argc, const char **argv)
{
  argc = argc;
  argv = argv;

  return -1;
}