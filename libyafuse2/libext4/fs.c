/**
 * fs.c - The filesystem entry of Ext4.
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
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <math.h>

#ifdef DEBUG
// Add code here
#endif

#include "include/base/debug.h"
#include "include/base/types.h"
#include "include/libio/io.h"
#include "include/fs.h"
#include "include/libext4/ext4.h"
#include "include/libext4/ext4_extents.h"
#include "include/libext4/ext4_jbd2.h"
#include "include/libext4/jbd2.h"
#include "include/libext4/libext4.h"

/*
 * Macro Definition
 */

/*
 * Type Definition
 */

/*
 * Global Variable Definition
 */
static struct file_system_type fs_file_type;
static struct super_block fs_sb;

/*
 * Function Declaration
 */
static int32_t fs_d_hash(const struct dentry *dentry, unsigned char *hash);
static int32_t fs_d_delete(const struct dentry *dentry);
static char* fs_d_dname(struct dentry *dentry, char *buffer, int32_t buflen);

static int32_t fs_max_size(uint8_t blocksize_bits);
static int32_t fs_statfs(struct dentry *dentry, struct kstatfs *buf);
static int32_t fs_fill_super(struct super_block *sb);
static struct dentry* fs_mount(struct file_system_type *type, int32_t flags,
                               const char *name, void *data);
static int32_t fs_umount(const char *name, int32_t flags);

static struct super_operations fs_super_opt = {
  fs_statfs,
};

static struct dentry_operations fs_dentry_opt = {
  fs_d_hash,
  fs_d_delete,
  fs_d_dname,
};

/*
 * Function Definition
 */
/*
 * Hash dentry
 */
static int32_t fs_d_hash(const struct dentry *dentry, unsigned char *hash)
{
  return 0;
}

/*
 * Delete dentry
 */
static int32_t fs_d_delete(const struct dentry *dentry)
{
  return 0;
}

/*
 * Name of dentry
 */
static char* fs_d_dname(struct dentry *dentry, char *buffer, int32_t buflen)
{
  return 0;
}

/*
 * Maximal file size
 */
static int32_t fs_max_size(uint8_t blocksize_bits)
{
  int32_t res;

  res = (1LL << 32) - 1;
  res <<= blocksize_bits;

  return res;
}

/*
 * Status of filesystem
 */
static int32_t fs_statfs(struct dentry *dentry, struct kstatfs *buf)
{
  return 0;
}

/*
 * Fill in superblock
 */
static int32_t fs_fill_super(struct super_block *sb)
{
  struct ext4_super_block ext4_sb;
  uint32_t len;
  int32_t ret;

  /*
   * Fill in Ext4 superblock
   */
  memset((void *)&ext4_sb, 0, sizeof(struct ext4_super_block));
  ret = ext4_fill_sb(&ext4_sb);
  if (ret != 0) {
    return -1;
  }

  /*
   * Fill in superblock
   */
  sb->s_blocksize_bits = (uint8_t)ext4_sb.s_log_block_size;
  sb->s_blocksize = (uint32_t)pow((double)2, (double)(10 + ext4_sb.s_log_block_size));
  sb->s_maxbytes = (int32_t)fs_max_size(sb->s_blocksize_bits);
  sb->s_type = (struct file_system_type *)&fs_file_type;
  sb->s_op = (const struct super_operations *)&fs_super_opt;
  sb->s_flags = (uint32_t)ext4_sb.s_flags;
  sb->s_magic = (uint32_t)ext4_sb.s_magic;
  sb->s_root = (struct dentry *)NULL; // Add code here
  sb->s_count = (int32_t)1;

  len = (uint32_t)(sizeof(sb->s_id) >= (strlen(fs_file_type.name) + 1) ? strlen(fs_file_type.name) + 1 : sizeof(sb->s_id));
  memcpy((void *)(sb->s_id), (const void *)(fs_file_type.name), len);
  sb->s_id[len - 1] = '\0';

  len = (uint32_t)(sizeof(sb->s_uuid) >= sizeof(ext4_sb.s_uuid) ? sizeof(ext4_sb.s_uuid) : sizeof(sb->s_uuid));
  memcpy((void *)(sb->s_uuid), (const void *)(ext4_sb.s_uuid), len);

  sb->s_d_op = (const struct dentry_operations *)&fs_dentry_opt;

  return 0;
}

/*
 * Mount filesystem
 */
static struct dentry* fs_mount(struct file_system_type *type, int32_t flags,
                               const char *name, void *data)
{
  int32_t ret;

  if (!type || !name) {
    return NULL;
  }

  /*
   * Open filesystem
   */
  ret = io_open(name);
  if (ret != 0) {
    return NULL;
  }

  /*
   * Fill in superblock
   */
  memset((void *)&fs_sb, 0, sizeof(struct super_block));
  ret = fs_fill_super(&fs_sb);
  if (ret != 0) {
    goto fs_mount_fail;
  }

 fs_mount_fail:

  memset((void *)&fs_sb, 0, sizeof(struct super_block));

  (void)io_close();

  return NULL;
}

/*
 * Unmount filesystem
 */
static int32_t fs_umount(const char *name, int32_t flags)
{
  if (!name) {
    return -1;
  }

  memset((void *)&fs_sb, 0, sizeof(struct super_block));

  (void)io_close();

  return 0;
}

/*
 * Init filesystem type
 */
#ifdef CMAKE_COMPILER_IS_GNUCC
struct file_system_type* fs_file_system_type_init(const char *type, int32_t flags)
#else
__declspec(dllexport) struct file_system_type* fs_file_system_type_init(const char *type, int32_t flags)
#endif /* CMAKE_COMPILER_IS_GNUCC */
{
  if (!type) {
    return NULL;
  }

  memset((void *)&fs_file_type, 0, sizeof(struct file_system_type));
  fs_file_type.name = type;
  fs_file_type.fs_flags = flags;
  fs_file_type.mount = fs_mount;
  fs_file_type.umount = fs_umount;

  return &fs_file_type;
}
