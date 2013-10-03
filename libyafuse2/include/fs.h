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
#include <stdint.h>
#include <time.h>

#ifdef DEBUG
// Add code here
#endif

#include "include/base/types.h"

/*
 * Macro Definition
 */
#define DNAME_INLINE_LEN 40

#define IS_ROOT(x) ((x) == (x)->d_parent)

/*
 * Type Definition
 */
struct qstr;
struct fs_timespec;
struct vfsmount;
struct mount;
struct fsid_t;
struct kstatfs;
struct kstat;
struct path;
struct file;
struct iattr;
struct file_system_type;
struct super_block;
struct dentry;
struct inode;
struct file_operations;
struct super_operations;
struct dentry_operations;
struct inode_operations;

struct qstr {
  uint32_t hash;
  uint32_t len;
  const unsigned char *name;
};

struct fs_timespec
{
  int32_t tv_sec;
  int32_t tv_nsec;
};

struct vfsmount {
  struct dentry *mnt_root;
  struct super_block *mnt_sb;
  int32_t mnt_flags;
};

struct mount {
  struct dentry *mnt_mountpoint;
  struct vfsmount mnt;
  int32_t mnt_count;
  const char *mnt_devname;
};

struct fsid_t {
  int32_t val[2];
};

struct kstatfs {
  int32_t f_type;
  int32_t f_bsize;
  uint32_t f_blocks;
  uint32_t f_bfree;
  uint32_t f_bavail;
  uint32_t f_files;
  uint32_t f_ffree;
  struct fsid_t f_fsid;
  int32_t f_namelen;
  int32_t f_frsize;
  int32_t f_flags;
  int32_t f_spare[4];
};

struct kstat {
  uint32_t ino;
  uint16_t mode;
  uint32_t nlink;
  uint32_t uid;
  uint32_t gid;
  int32_t size;
  struct fs_timespec atime;
  struct fs_timespec mtime;
  struct fs_timespec ctime;
  uint32_t blksize;
  uint32_t blocks;
};

struct path {
  struct vfsmount *mnt;
  struct dentry *dentry;
};

struct file {
  struct path f_path;
#define f_dentry f_path.dentry
  struct inode *f_inode;
  const struct file_operations *f_op;
  uint32_t f_flags;
  uint32_t f_mode;
  int32_t f_pos;
  uint32_t f_version;
};

struct iattr {
  uint32_t ia_valid;
  uint16_t ia_mode;
  uint32_t ia_uid;
  uint32_t ia_gid;
  int32_t ia_size;
  struct fs_timespec ia_atime;
  struct fs_timespec ia_mtime;
  struct fs_timespec ia_ctime;
  struct file *ia_file;
};

struct file_system_type {
  const char *name;
  int32_t fs_flags;
  struct dentry* (*mount) (struct file_system_type *type, int32_t flags,
                           const char *name, void *data);
  int32_t (*umount) (const char *name, int32_t flags);
};

struct super_block {
  uint8_t                        s_blocksize_bits;
  uint32_t                       s_blocksize;
  int32_t                        s_maxbytes;
  struct file_system_type        *s_type;
  const struct super_operations  *s_op;
  uint32_t                       s_flags;
  uint32_t                       s_magic;
  struct dentry                  *s_root;
  int32_t                        s_count;
  struct list_head               s_inodes;
  char                           s_id[32];
  uint8_t                        s_uuid[16];
  const struct dentry_operations *s_d_op;
};

struct dentry {
  struct dentry                  *d_parent;
  struct qstr                    *d_name;
  struct inode                   *d_inode;
  const struct dentry_operations *d_op;
  struct super_block             *d_sb;
  struct list_head               d_child;
  struct list_head               d_subdirs;
  struct list_head               d_alias;
};

struct inode {
  uint16_t                      i_mode;
  uint16_t                      i_opflags;
  uint32_t                      i_uid;
  uint32_t                      i_gid;
  uint32_t                      i_flags;
  const struct inode_operations *i_op;
  struct super_block            *i_sb;
  uint32_t                      i_ino;
  struct fs_timespec            i_atime;
  struct fs_timespec            i_mtime;
  struct fs_timespec            i_ctime;
  uint16_t                      i_bytes;
  uint32_t                      i_blocks;
  int32_t                       i_size;
  struct hlist_node             i_hash;
  struct list_head              i_dentry;
  uint32_t                      i_count;
  uint32_t                      i_blkbits;
  uint32_t                      i_version;
  const struct file_operations  *i_fop;
};

struct file_operations {
  ssize_t (*read) (struct file *, char *, size_t, int32_t *);
  ssize_t (*write) (struct file *, const char *, size_t, int32_t *);
  int32_t (*open) (struct inode *, struct file *);
  int32_t (*release) (struct inode *, struct file *);
};

struct super_operations {
  struct inode* (*alloc_inode) (struct super_block *);
  void (*destroy_inode) (struct inode *);
  int32_t (*statfs) (struct dentry *, struct kstatfs *);
};

struct dentry_operations {
  int32_t (*d_hash) (const struct dentry *, const struct inode *, struct qstr *);
  void (*d_release) (struct dentry *);
  char *(*d_dname) (struct dentry *, char *, int32_t);
};

struct inode_operations {
  struct dentry * (*lookup) (struct inode *, struct dentry *, uint32_t);
  int32_t (*permission) (struct inode *, int32_t);
  struct posix_acl * (*get_acl) (struct inode *, int32_t);
  int32_t (*create) (struct inode *, struct dentry *, uint8_t, bool);
  int32_t (*link) (struct dentry *, struct inode *, struct dentry *);
  int32_t (*unlink) (struct inode *, struct dentry *);
  int32_t (*symlink) (struct inode *, struct dentry *, const char *);
  int32_t (*mkdir) (struct inode *, struct dentry *, uint8_t);
  int32_t (*rmdir) (struct inode *, struct dentry *);
  int32_t (*mknod) (struct inode *, struct dentry *, uint8_t, dev_t);
  int32_t (*rename) (struct inode *, struct dentry *, struct inode *, struct dentry *);
  int32_t (*setattr) (struct dentry *, struct iattr *);
  int32_t (*getattr) (struct vfsmount *mnt, struct dentry *, struct kstat *);
  int32_t (*setxattr) (struct dentry *, const char *, const void *, size_t, int32_t);
  ssize_t (*getxattr) (struct dentry *, const char *, void *, size_t);
  ssize_t (*listxattr) (struct dentry *, char *, size_t);
  int32_t (*removexattr) (struct dentry *, const char *);
  int32_t (*update_time) (struct inode *, struct fs_timespec *, int32_t);
};

/*
 * Function Declaration
 */
typedef struct file_system_type* (*fs_file_system_type_init_t) (const char *type, int32_t flags);

#endif /* _FS_H */