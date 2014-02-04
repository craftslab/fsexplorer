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
#include <time.h>

/*
 * Macro Definition
 */
#define FS_TYPE_EXT4 "ext4"
#define FS_TYPE_FAT  "fat"

/*
 * Type Definition
 */
enum libfs_ftype {
  FT_UNKNOWN  = 0,
  FT_REG_FILE = 1,
  FT_DIR      = 2,
  FT_CHRDEV   = 3,
  FT_BLKDEV   = 4,
  FT_FIFO     = 5,
  FT_SOCK     = 6,
  FT_SYMLINK  = 7,
  FT_MAX      = 8
};

enum libfs_imode {
  IXOTH  = 0x1,
  IWOTH  = 0x2,
  IROTH  = 0x4,
  IXGRP  = 0x8,
  IWGRP  = 0x10,
  IRGRP  = 0x20,
  IXUSR  = 0x40,
  IWUSR  = 0x80,
  IRUSR  = 0x100,
  ISVTX  = 0x200,
  ISGID  = 0x400,
  ISUID  = 0x800,
  IFIFO  = 0x1000,
  IFCHR  = 0x2000,
  IFDIR  = 0x4000,
  IFBLK  = 0x6000,
  IFREG  = 0x8000,
  IFLNK  = 0xA000,
  IFSOCK = 0xC000,
  IFMAX  = 0xFFFF
};

struct libfs_timespec
{
  int64_t tv_sec;
  int64_t tv_nsec;
};

struct fs_fsid_t {
  int32_t val[2];
};

struct fs_kstatfs {
  enum libfs_ftype f_type;
  int64_t f_bsize;
  uint64_t f_blocks;
  uint64_t f_bfree;
  uint64_t f_bavail;
  uint64_t f_files;
  uint64_t f_ffree;
  struct fs_fsid_t f_fsid;
  int64_t f_namelen;
  int64_t f_frsize;
  int64_t f_flags;
  int64_t f_spare[4];
};

struct fs_dirent {
  uint64_t d_ino;
  int64_t d_off;
  uint16_t d_reclen;
  enum libfs_ftype d_type;
  char d_name[256];
};

struct fs_kstat {
  uint64_t ino;
  enum libfs_imode mode;
  uint32_t nlink;
  uint32_t uid;
  uint32_t gid;
  int64_t size;
  struct libfs_timespec atime;
  struct libfs_timespec mtime;
  struct libfs_timespec ctime;
  uint64_t blksize;
  uint64_t blocks;
};

struct fs_opt_t {
  int32_t (*mount) (const char *devname, const char *dirname, const char *type, int32_t flags, struct fs_dirent *dirent);
  int32_t (*umount) (const char *dirname, int32_t flags);
  int32_t (*statfs) (const char *pathname, struct fs_kstatfs *buf);
  int32_t (*stat) (uint64_t ino, struct fs_kstat *buf);
  int32_t (*getdents) (uint64_t ino, struct fs_dirent **dirents, uint32_t *dirents_num);
};

/*
 * Function Declaration
 */
typedef int32_t (*fs_opt_init_t) (struct fs_opt_t *fs_opt);

#endif /* _LIBFS_H */
