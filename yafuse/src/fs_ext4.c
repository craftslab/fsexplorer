/**
 * fs_ext4.c - The entry of Ext4 filesystem.
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
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef DEBUG
// Add code here
#endif

#include "include/debug.h"
#include "include/types.h"
#include "include/libext4/ext4.h"
#include "include/libext4/ext4_extents.h"
#include "include/libext4/ext4_jbd2.h"
#include "include/libext4/jbd2.h"
#include "include/libext4/libext4.h"
#include "include/libio/io.h"

#include "filesystem.h"
#include "fs_ext4.h"

/*
 * Macro Definition
 */
#define FS_EXT4_STAT_INO_DELIM_L  '<'
#define FS_EXT4_STAT_INO_DELIM_R  '>'

/*
 * Type Definition
 */
typedef struct {
  uint32_t ino;
  const char *name;
} fs_ext4_cwd_t;

typedef struct {
  int32_t mounted;
  fs_ext4_cwd_t cwd;
  struct ext4_super_block *sb;
  int32_t bg_groups;
  struct ext4_group_desc_min *bg_desc;
} fs_ext4_info_t;

/*
 * Function Declaration
 */
static int32_t fs_parse_ino(const char *name, uint32_t *ino);

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
 * Ext4 filesystem operation table
 */
fs_opt_t fs_opt_tbl_ext4[FS_OPT_TBL_NUM_MAX] = {
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

static fs_ext4_info_t ext4_info;

/*
 * Function Definition
 */
static int32_t fs_parse_ino(const char *name, uint32_t *ino)
{
  char *buf = NULL;
  size_t len_name = 0, len_buf = 0;
  int32_t val = 0;
  int32_t ret = 0;

  if (name == NULL || ino == NULL) {
    return -1;
  }

  /*
   * len_name <= len(FS_EXT4_STAT_INO_DELIM_L) + len(FS_EXT4_STAT_INO_DELIM_R)
   */
  len_name = strlen(name);
  if (len_name <= 2) {
    return -1;
  }

  if (name[0] != FS_EXT4_STAT_INO_DELIM_L && name[len_name - 1] != FS_EXT4_STAT_INO_DELIM_R) {
    return -1;
  }

  /*
   * len_buf = len_name - (len(FS_EXT4_STAT_INO_DELIM_L) + len(FS_EXT4_STAT_INO_DELIM_R)) + 1;
   */
  len_buf = len_name - (1 + 1) + 1;

  buf = (char *)malloc(len_buf);
  if (buf == NULL) {
    return -1;
  }
  memset((void *)buf, 0, len_buf);
  strncpy((void *)buf, (const void *)&name[1], len_buf - 1);

  val = atoi((const char *)buf);
  if (val < EXT4_ROOT_INO) {
    ret = -1;
    goto fs_parse_ino_done;
  }

  *ino = (uint32_t)val;

  ret = 0;

 fs_parse_ino_done:

  if (buf != NULL) free(buf);

  return ret;
}

static int32_t fs_do_mount(int32_t argc, const char **argv)
{
  const char *name = NULL;
  struct ext4_super_block *sb = NULL;
  int32_t bg_groups = 0;
  struct ext4_group_desc_min *bg_desc = NULL;
  int32_t ret = -1;

  if (argc < 2 || argv == NULL) {
    error("invalid args!");
    return -1;
  }

  name = argv[1];
  if (name == NULL) {
    error("invalid args!");
    return -1;
  }

  if (ext4_info.mounted) {
    info("umount ext4 filesystem first.");
    return 0;
  }

  /*
   * Open Ext4 image
   */
  ret = io_open(name);
  if (ret != 0) {
    error("failed to open io!");
    return -1;
  }

  /*
   * Fill in Ext4 superblock
   */
  sb = (struct ext4_super_block *)malloc(sizeof(struct ext4_super_block));
  if (sb == NULL) {
    error("failed to malloc sb!");
    ret = -1;
    goto fs_do_mount_fail;
  }
  memset((void *)sb, 0, sizeof(struct ext4_super_block));

  ret = ext4_fill_sb(sb);
  if (ret != 0) {
    goto fs_do_mount_fail;
  }

  /*
   * Fill in Ext4 block group number
   */
  ret = ext4_fill_bg_groups((const struct ext4_super_block *)sb, &bg_groups);
  if (ret != 0) {
    goto fs_do_mount_fail;
  }

  /*
   * Fill in Ext4 block group descriptor
   */
  if (sb->s_feature_incompat & EXT4_FEATURE_INCOMPAT_64BIT
      && sb->s_desc_size > EXT4_MIN_DESC_SIZE) {
    error("not support size of 2^64 blocks!");
    ret = -1;
    goto fs_do_mount_fail;
  }

  bg_desc = (struct ext4_group_desc_min *)malloc(sb->s_desc_size * bg_groups);
  if (bg_desc == NULL) {
    error("failed to malloc bg desc!");
    ret = -1;
    goto fs_do_mount_fail;
  }
  memset((void *)bg_desc, 0, sb->s_desc_size * bg_groups);

  ret = ext4_fill_bg_desc((const struct ext4_super_block *)sb, bg_groups, bg_desc);
  if (ret != 0) {
    goto fs_do_mount_fail;
  }

  /*
   * Init Ext4 filesytem info
   */
  ext4_info.mounted = 1;
  ext4_info.cwd.ino = EXT4_ROOT_INO;
  ext4_info.cwd.name = FS_ROOT_DIR;
  ext4_info.sb = sb;
  ext4_info.bg_groups = bg_groups;
  ext4_info.bg_desc = bg_desc;

  info("mount ext4 filesystem successfully.");

  return 0;

 fs_do_mount_fail:

  info("failed to mount ext4 filesystem.");

  if (bg_desc != NULL) free(bg_desc);
  if (sb != NULL) free(sb);

  io_close();

  memset((void *)&ext4_info, 0, sizeof(fs_ext4_info_t));

  return ret;
}

static int32_t fs_do_umount(int32_t argc, const char **argv)
{
  argc = argc;
  argv = argv;

  if (ext4_info.mounted) {
    info("umount ext4 filesystem successfully.");
  }

  if (ext4_info.bg_desc != NULL) free(ext4_info.bg_desc);
  if (ext4_info.sb != NULL) free(ext4_info.sb);

  io_close();

  memset((void *)&ext4_info, 0, sizeof(fs_ext4_info_t));

  return 0;
}

static int32_t fs_do_stats(int32_t argc, const char **argv)
{
  argc = argc;
  argv = argv;

  if (ext4_info.sb == NULL
      || ext4_info.bg_groups <= 0
      || ext4_info.bg_desc == NULL) {
    error("failed to stats ext4 filesystem!");
    return -1;
  }

  ext4_show_stats((const struct ext4_super_block *)ext4_info.sb,
                  ext4_info.bg_groups,
                  (const struct ext4_group_desc_min *)ext4_info.bg_desc);

  return 0;
}

static int32_t fs_do_stat(int32_t argc, const char **argv)
{
  uint32_t inode_num = 0;
  struct ext4_inode inode;
  int32_t ret = 0;

  argc = argc;
  argv = argv;

  if (argc != 2 || argv == NULL) {
    error("invalid args!");
    return -1;
  }

  /*
   * Parse args
   * e.g., 'stat name' or 'stat <ino>'
   */
  ret = fs_parse_ino(argv[1], &inode_num);
  if (ret != 0) {
    ret = ext4_name2ino((const struct ext4_super_block *)ext4_info.sb,
                        (const struct ext4_group_desc_min *)ext4_info.bg_desc,
                        argv[1],
                        &inode_num);
    if (ret != 0) {
      error("invalid args!");
      return -1;
    }
  }

  /*
   * Fill in Ext4 inode
   */
  memset((void *)&inode, 0, sizeof(struct ext4_inode));

  ext4_fill_inode((const struct ext4_super_block *)ext4_info.sb,
                  (const struct ext4_group_desc_min *)ext4_info.bg_desc,
                  inode_num,
                  &inode);

  /*
   * Show Ext4 inode stat
   */
  ext4_show_inode_stat((const struct ext4_super_block *)ext4_info.sb,
                       inode_num,
                       (const struct ext4_inode *)&inode);

  return 0;
}

static int32_t fs_do_pwd(int32_t argc, const char **argv)
{
  argc = argc;
  argv = argv;

  fprintf(stdout, "%s\n", ext4_info.cwd.name);

  return 0;
}

static int32_t fs_do_cd(int32_t argc, const char **argv)
{
  argc = argc;
  argv = argv;

  return -1;
}

static int32_t fs_do_ls(int32_t argc, const char **argv)
{
  const char *name __attribute__((unused)) = NULL;

  if (argc > 2 || argv == NULL) {
    error("invalid args!");
    return -1;
  }

  if (argc == 2) {
    name = argv[1];
  }

  return 0;
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
