/**
 * libfs.h - The header of libfs.
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

#ifndef _LIBFS_H
#define _LIBFS_H

#include <stdint.h>

/*
 * Macro Definition
 */

/*
 * Type Definition
 */
struct fs_fsid_t {
  int32_t val[2];
};

struct fs_kstatfs {
  int32_t f_type;
  int32_t f_bsize;
  uint32_t f_blocks;
  uint32_t f_bfree;
  uint32_t f_bavail;
  uint32_t f_files;
  uint32_t f_ffree;
  struct fs_fsid_t f_fsid;
  int32_t f_namelen;
  int32_t f_frsize;
  int32_t f_flags;
  int32_t f_spare[4];
};

struct fs_dirent {
  uint32_t d_ino;
  uint32_t d_off;
  uint16_t d_reclen;
  uint8_t d_type;
  char d_name[256];
};

struct fs_kstat {
  uint32_t ino;
  uint16_t mode;
  uint32_t nlink;
  uint32_t uid;
  uint32_t gid;
  int32_t size;
  struct timespec atime;
  struct timespec mtime;
  struct timespec ctime;
  uint32_t blksize;
  uint32_t blocks;
};

struct fs_opt_t {
  int32_t (*mount) (const char *dev_name, const char *dir_name, const char *type, uint32_t flags, void *data);
  int32_t (*umount) (const char *name, int32_t flags);
  int32_t (*statfs) (const char *pathname, struct fs_kstatfs *buf);
  int32_t (*stat) (const char *filename, struct fs_kstat *statbuf);
  int32_t (*getdents) (uint32_t fd, struct fs_dirent *dirent, uint32_t count);
  int32_t (*getcwd) (char *buf, uint32_t size);
  int32_t (*chdir) (const char *filename);
};

/*
 * Function Declaration
 */
typedef int32_t (*fs_opt_init_t) (struct fs_opt_t *fs_opt);

#endif /* _LIBFS_H */
