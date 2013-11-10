/**
 * debug.c - debugging Ext4.
 *
 * Copyright (c) 2013-2014 angersax@gmail.com
 *
 * This program/include file is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program/include file is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program (in the main directory of the distribution
 * in the file COPYING); if not, write to the Free Software
 * Foundation,Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "config.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#ifdef DEBUG
#define DEBUG_LIBEXT4_DEBUG
#endif

#include "include/base/debug.h"
#include "include/base/types.h"
#include "include/libio/io.h"
#include "include/libext4/libext4.h"

/*
 * Macro Definition
 */
#define EXT4_DUMMY_STR  "<none>"

/*
 * Type Definition
 */

/*
 * Global Variable Definition
 */

/*
 * Function Declaration
 */

/*
 * Function Definition
 */
void ext4_show_sb_stat(struct ext4_super_block *sb)
{
  int32_t i = 0;
  uint32_t val = 0;
  const char *str = NULL;
  time_t tm = 0;

  fprintf(stdout, "Total inode count              : %u\n", sb->s_inodes_count);
  fprintf(stdout, "Total block count              : %llu\n", ((__le64)sb->s_blocks_count_hi << 32) | (__le64)sb->s_blocks_count_lo);
  fprintf(stdout, "Reserved block count           : %llu\n", ((__le64)sb->s_r_blocks_count_hi << 32) | (__le64)sb->s_r_blocks_count_lo);
  fprintf(stdout, "Free block count               : %llu\n", ((__le64)sb->s_free_blocks_count_hi << 32) | (__le64)sb->s_free_blocks_count_lo);
  fprintf(stdout, "Free inode count               : %u\n", sb->s_free_inodes_count);
  fprintf(stdout, "First data block               : %u\n", sb->s_first_data_block);

  val = (uint32_t)pow((double)2, (double)(10 + sb->s_log_block_size));
  fprintf(stdout, "Block size                     : %u\n", val);

  val = (uint32_t)pow((double)2, (double)(10 + sb->s_obso_log_frag_size));
  fprintf(stdout, "Fragment size (obsolete)       : %u\n", val);

  fprintf(stdout, "Blocks per group               : %u\n", sb->s_blocks_per_group);
  fprintf(stdout, "Fragments per group (obsolete) : %u\n", sb->s_obso_frags_per_group);
  fprintf(stdout, "Inodes per group               : %u\n", sb->s_inodes_per_group);

  fprintf(stdout, "Mount time                     : ");
  if (sb->s_mtime != 0) {
    tm = (time_t)sb->s_mtime;
    fprintf(stdout, "%s", ctime(&tm));
  } else {
    fprintf(stdout, EXT4_DUMMY_STR);
    fprintf(stdout, "\n");
  }

  fprintf(stdout, "Write time                     : ");
  if (sb->s_wtime != 0) {
    tm = (time_t)sb->s_wtime;
    fprintf(stdout, "%s", ctime(&tm));
  } else {
    fprintf(stdout, EXT4_DUMMY_STR);
    fprintf(stdout, "\n");
  }

  fprintf(stdout, "Mount count                    : %u\n", sb->s_mnt_count);
  fprintf(stdout, "Maximum mount count            : %u\n", sb->s_max_mnt_count);
  fprintf(stdout, "Magic signature                : 0x%X\n", sb->s_magic);

  fprintf(stdout, "File system state              : ");
  str = NULL;
  if (sb->s_state & EXT4_VALID_FS) {
    str = "cleanly umounted";
    fprintf(stdout, "%s ", str);
  }
  if (sb->s_state & EXT4_ERROR_FS) {
    str = "errors detected";
    fprintf(stdout, "%s ", str);
  }
  if (sb->s_state & EXT4_ORPHAN_FS) {
    str = "orphans being recovered";
    fprintf(stdout, "%s ", str);
  }
  if (!str) {
    str = EXT4_DUMMY_STR;
    fprintf(stdout, "%s", str);
  }
  fprintf(stdout, "\n");

  switch (sb->s_errors) {
  case EXT4_ERRORS_CONTINUE:
    str = "continue";
    break;
  case EXT4_ERRORS_RO:
    str = "remount read-only";
    break;
  case EXT4_ERRORS_PANIC:
    str = "panic";
    break;
  default:
    str = EXT4_DUMMY_STR;
    break;
  }
  fprintf(stdout, "Errors behaviour               : %s\n", str);

  fprintf(stdout, "Minor revision level           : %u\n", sb->s_minor_rev_level);
  fprintf(stdout, "Last checked                   : %u\n", sb->s_lastcheck);
  fprintf(stdout, "Check interval                 : %u\n", sb->s_checkinterval);

  switch (sb->s_creator_os) {
  case EXT4_OS_LINUX:
    str = "Linux";
    break;
  case EXT4_OS_HURD:
    str = "Hurd";
    break;
  case EXT4_OS_MASIX:
    str = "Masix";
    break;
  case EXT4_OS_FREEBSD:
    str = "FreeBSD";
    break;
  case EXT4_OS_LITES:
    str = "Lites";
    break;
  default:
    str = EXT4_DUMMY_STR;
    break;
  }
  fprintf(stdout, "OS type                        : %s\n", str);

  switch (sb->s_rev_level) {
  case EXT4_GOOD_OLD_REV:
    str = "original format";
    break;
  case EXT4_DYNAMIC_REV:
    str = "v2 format w/ dynamic inode sizes";
    break;
  default:
    str = EXT4_DUMMY_STR;
    break;
  }
  fprintf(stdout, "Revision level                 : %s\n", str);

  fprintf(stdout, "Reserved blocks uid            : %u\n", sb->s_def_resuid);
  fprintf(stdout, "Reserved blocks gid            : %u\n", sb->s_def_resgid);

  if (sb->s_rev_level == EXT4_DYNAMIC_REV) {
    fprintf(stdout, "\n");
    fprintf(stdout, "First non-reserved inode    : %u\n", sb->s_first_ino);
    fprintf(stdout, "Inode size                  : %u\n", sb->s_inode_size);
    fprintf(stdout, "Block group number          : %u\n", sb->s_block_group_nr);

    str = NULL;
    fprintf(stdout, "Compatible feature          : ");
    if (sb->s_feature_compat & EXT4_FEATURE_COMPAT_DIR_PREALLOC) {
      str = "directory preallocation";
      fprintf(stdout, "%s, ", str);
    }
    if (sb->s_feature_compat & EXT4_FEATURE_COMPAT_IMAGIC_INODES) {
      str = "imagic inodes";
      fprintf(stdout, "%s, ", str);
    }
    if (sb->s_feature_compat & EXT4_FEATURE_COMPAT_HAS_JOURNAL) {
      str = "has a journal";
      fprintf(stdout, "%s, ", str);
    }
    if (sb->s_feature_compat & EXT4_FEATURE_COMPAT_EXT_ATTR) {
      str = "support extended attributes";
      fprintf(stdout, "%s, ", str);
    }
    if (sb->s_feature_compat & EXT4_FEATURE_COMPAT_RESIZE_INODE) {
      str = "has reserved GDT blocks for filesystem expansion";
      fprintf(stdout, "%s, ", str);
    }
    if (sb->s_feature_compat & EXT4_FEATURE_COMPAT_DIR_INDEX) {
      str = "has directory indices";
      fprintf(stdout, "%s, ", str);
    }
    if (sb->s_feature_compat & 0x40) {
      str = "lazy BG";
      fprintf(stdout, "%s, ", str);
    }
    if (sb->s_feature_compat & 0x80) {
      str = "exclude inode";
      fprintf(stdout, "%s, ", str);
    }
    if (str == NULL) {
      fprintf(stdout, EXT4_DUMMY_STR);
    }
    fprintf(stdout, "\n");

    str = NULL;
    fprintf(stdout, "Incompatible feature        : ");
    if (sb->s_feature_incompat & EXT4_FEATURE_INCOMPAT_COMPRESSION) {
      str = "compression";
      fprintf(stdout, "%s, ", str);
    }
    if (sb->s_feature_incompat & EXT4_FEATURE_INCOMPAT_FILETYPE) {
      str = "directory entries record the file type";
      fprintf(stdout, "%s, ", str);
    }
    if (sb->s_feature_incompat & EXT4_FEATURE_INCOMPAT_RECOVER) {
      str = "need recovery";
      fprintf(stdout, "%s, ", str);
    }
    if (sb->s_feature_incompat & EXT4_FEATURE_INCOMPAT_JOURNAL_DEV) {
      str = "has a separate journal device";
      fprintf(stdout, "%s, ", str);
    }
    if (sb->s_feature_incompat & EXT4_FEATURE_INCOMPAT_META_BG) {
      str = "meta block groups";
      fprintf(stdout, "%s, ", str);
    }
    if (sb->s_feature_incompat & EXT4_FEATURE_INCOMPAT_EXTENTS) {
      str = "use extents for file";
      fprintf(stdout, "%s, ", str);
    }
    if (sb->s_feature_incompat & EXT4_FEATURE_INCOMPAT_64BIT) {
      str = "support size of 2^64 blocks";
      fprintf(stdout, "%s, ", str);
    }
    if (sb->s_feature_incompat & EXT4_FEATURE_INCOMPAT_MMP) {
      str = "multiple mount protection";
      fprintf(stdout, "%s, ", str);
    }
    if (sb->s_feature_incompat & EXT4_FEATURE_INCOMPAT_FLEX_BG) {
      str = "flexible block groups";
      fprintf(stdout, "%s, ", str);
    }
    if (sb->s_feature_incompat & EXT4_FEATURE_INCOMPAT_EA_INODE) {
      str = "support inode with large extended attributes";
      fprintf(stdout, "%s, ", str);
    }
    if (sb->s_feature_incompat & EXT4_FEATURE_INCOMPAT_DIRDATA) {
      str = "data in directory entry";
      fprintf(stdout, "%s, ", str);
    }
    if (str == NULL) {
      fprintf(stdout, EXT4_DUMMY_STR);
    }
    fprintf(stdout, "\n");

    str = NULL;
    fprintf(stdout, "Readonly-compatible feature : ");
    if (sb->s_feature_ro_compat & EXT4_FEATURE_RO_COMPAT_SPARSE_SUPER) {
      str = "sparse superblocks";
      fprintf(stdout, "%s, ", str);
    }
    if (sb->s_feature_ro_compat & EXT4_FEATURE_RO_COMPAT_LARGE_FILE) {
      str = "support file greater than 2GiB";
      fprintf(stdout, "%s, ", str);
    }
    if (sb->s_feature_ro_compat & EXT4_FEATURE_RO_COMPAT_BTREE_DIR) {
      str = "has btree directory";
      fprintf(stdout, "%s, ", str);
    }
    if (sb->s_feature_ro_compat & EXT4_FEATURE_RO_COMPAT_HUGE_FILE) {
      str = "support huge file";
      fprintf(stdout, "%s, ", str);
    }
    if (sb->s_feature_ro_compat & EXT4_FEATURE_RO_COMPAT_GDT_CSUM) {
      str = "group descriptors have checksums";
      fprintf(stdout, "%s, ", str);
    }
    if (sb->s_feature_ro_compat & EXT4_FEATURE_RO_COMPAT_DIR_NLINK) {
      str = "not support old ext3 32,000 subdirectory limit";
      fprintf(stdout, "%s, ", str);
    }
    if (sb->s_feature_ro_compat & EXT4_FEATURE_RO_COMPAT_EXTRA_ISIZE) {
      str = "support large inodes";
      fprintf(stdout, "%s, ", str);
    }
    if (sb->s_feature_ro_compat & 0x80) {
      str = "has a snapshot";
      fprintf(stdout, "%s, ", str);
    }
    if (str == NULL) {
      fprintf(stdout, EXT4_DUMMY_STR);
    }
    fprintf(stdout, "\n");

    fprintf(stdout, "UUID                        : ");
    for (i = 0; i < 16; ++i) {
      fprintf(stdout, "%x", sb->s_uuid[i]);
    }
    fprintf(stdout, "\n");

    fprintf(stdout, "Volume name                 : ");
    if (sb->s_volume_name[0] == '\0') {
      fprintf(stdout, EXT4_DUMMY_STR);
    } else {
      for (i = 0; i < 16; ++i) {
        fprintf(stdout, "%c", sb->s_volume_name[i]);
      }
    }
    fprintf(stdout, "\n");

    fprintf(stdout, "Last mounted on             : ");
    if (sb->s_last_mounted[0] == '\0') {
      fprintf(stdout, EXT4_DUMMY_STR);
    } else {
      fprintf(stdout, "%s", sb->s_last_mounted);
    }
    fprintf(stdout, "\n");

    fprintf(stdout, "Bitmap algorithm usage      : %u\n", sb->s_algorithm_usage_bitmap);
  }

  if (sb->s_feature_compat & EXT4_FEATURE_COMPAT_DIR_PREALLOC) {
    fprintf(stdout, "\n");
    fprintf(stdout, "Blocks preallocated for files : %u\n", sb->s_prealloc_blocks);
    fprintf(stdout, "Blocks preallocated for dirs  : %u\n", sb->s_prealloc_dir_blocks);
    fprintf(stdout, "Reserved GDT blocks           : %u\n", sb->s_reserved_gdt_blocks);
  }

  if (sb->s_feature_compat & EXT4_FEATURE_COMPAT_HAS_JOURNAL) {
    fprintf(stdout, "\n");
    fprintf(stdout, "Journal UUID                         : ");
    for (i = 0; i < 16; ++i) {
      fprintf(stdout, "%x", sb->s_journal_uuid[i]);
    }
    fprintf(stdout, "\n");

    fprintf(stdout, "Journal inode                        : %u\n", sb->s_journal_inum);
    fprintf(stdout, "Journal device                       : %u\n", sb->s_journal_dev);
    fprintf(stdout, "Orphaned inodes to delete            : %u\n", sb->s_last_orphan);

    fprintf(stdout, "HTREE hash seed                      : ");
    for (i = 0; i < (4 * 4); ++i) {
      fprintf(stdout, "%x", *((const __u8 *)(sb->s_hash_seed) + i));
    }
    fprintf(stdout, "\n");

    switch (sb->s_def_hash_version) {
    case DX_HASH_LEGACY:
      str = "legacy";
      break;
    case DX_HASH_HALF_MD4:
      str = "half MD4";
      break;
    case DX_HASH_TEA:
      str = "tea";
      break;
    case DX_HASH_LEGACY_UNSIGNED:
      str = "unsigned legacy";
      break;
    case DX_HASH_HALF_MD4_UNSIGNED:
      str = "unsigned half MD4";
      break;
    case DX_HASH_TEA_UNSIGNED:
      str = "unsigned tea";
      break;
    default:
      str = EXT4_DUMMY_STR;
      break;
    }
    fprintf(stdout, "Default hash version for dirs hashes : %s\n", str);

    fprintf(stdout, "Reserved char padding                : %u\n", sb->s_reserved_char_pad);
    fprintf(stdout, "Group descriptors size               : %u\n", sb->s_desc_size);

    str = NULL;
    fprintf(stdout, "Default mount options                : ");
    if (sb->s_default_mount_opts & EXT4_DEFM_DEBUG) {
      str = "print debugging info upon (re)mount";
      fprintf(stdout, "%s, ", str);
    }
    if (sb->s_default_mount_opts & EXT4_DEFM_BSDGROUPS) {
      str = "New files take the gid of the containing directory";
      fprintf(stdout, "%s, ", str);
    }
    if (sb->s_default_mount_opts & EXT4_DEFM_XATTR_USER) {
      str = "support userspace-provided extended attributes";
      fprintf(stdout, "%s, ", str);
    }
    if (sb->s_default_mount_opts & EXT4_DEFM_ACL) {
      str = "support POSIX access control lists (ACLs)";
      fprintf(stdout, "%s, ", str);
    }
    if (sb->s_default_mount_opts & EXT4_DEFM_UID16) {
      str = "not support 32-bit UIDs";
      fprintf(stdout, "%s, ", str);
    }
    if (sb->s_default_mount_opts & EXT4_DEFM_JMODE_DATA) {
      str = "all data and metadata are commited to the journal";
      fprintf(stdout, "%s, ", str);
    }
    if (sb->s_default_mount_opts & EXT4_DEFM_JMODE_ORDERED) {
      str = "all data are flushed to the disk before metadata are committed to the journal";
      fprintf(stdout, "%s, ", str);
    }
    if (sb->s_default_mount_opts & EXT4_DEFM_JMODE_WBACK) {
      str = "data ordering is not preserved which may be written after the metadata has been written";
      fprintf(stdout, "%s, ", str);
    }
    if (sb->s_default_mount_opts & EXT4_DEFM_NOBARRIER) {
      str = "disable write flushes";
      fprintf(stdout, "%s, ", str);
    }
    if (sb->s_default_mount_opts & EXT4_DEFM_BLOCK_VALIDITY) {
      str = "track which blocks in a filesystem are metadata";
      fprintf(stdout, "%s, ", str);
    }
    if (sb->s_default_mount_opts & EXT4_DEFM_DISCARD) {
      str = "enable DISCARD support";
      fprintf(stdout, "%s, ", str);
    }
    if (sb->s_default_mount_opts & EXT4_DEFM_NODELALLOC) {
      str = "disable delayed allocation";
      fprintf(stdout, "%s, ", str);
    }
    if (str == NULL) {
      fprintf(stdout, EXT4_DUMMY_STR);
    }
    fprintf(stdout, "\n");

    fprintf(stdout, "First metablock block group          : %u\n", sb->s_first_meta_bg);

    fprintf(stdout, "Filesystem-created time              : ");
    if (sb->s_mkfs_time != 0) {
      tm = (time_t)sb->s_mkfs_time;
      fprintf(stdout, "%s", ctime(&tm));
    } else {
      fprintf(stdout, EXT4_DUMMY_STR);
    }
    fprintf(stdout, "\n");

    fprintf(stdout, "Journal backup                       : ");
    for (i = 0; i < (17 * 4); ++i) {
      fprintf(stdout, "%x", *((const __u8 *)(sb->s_jnl_blocks) + i));
    }
    fprintf(stdout, "\n");
  }

  if (sb->s_feature_incompat & EXT4_FEATURE_INCOMPAT_64BIT) {
    fprintf(stdout, "\n");
    fprintf(stdout, "Required extra isize             : %u\n", sb->s_min_extra_isize);
    fprintf(stdout, "Desired extra isize              : %u\n", sb->s_want_extra_isize);

    str = NULL;
    fprintf(stdout, "Misc flags                       : ");
    if (sb->s_flags & EXT2_FLAGS_SIGNED_HASH) {
      str = "signed directory hash in use";
      fprintf(stdout, "%s, ", str);
    }
    if (sb->s_flags & EXT2_FLAGS_UNSIGNED_HASH) {
      str = "unsigned directory hash in use";
      fprintf(stdout, "%s, ", str);
    }
    if (sb->s_flags & EXT2_FLAGS_TEST_FILESYS) {
      str = "to test development code";
      fprintf(stdout, "%s, ", str);
    }
    if (sb->s_flags & 0x10) {
      str = "is snapshot";
      fprintf(stdout, "%s, ", str);
    }
    if (sb->s_flags & 0x20) {
      str = "fix snapshot";
      fprintf(stdout, "%s, ", str);
    }
    if (sb->s_flags & 0x40) {
      str = "fix exclusion";
      fprintf(stdout, "%s, ", str);
    }
    if (str == NULL) {
      fprintf(stdout, EXT4_DUMMY_STR);
    }
    fprintf(stdout, "\n");

    fprintf(stdout, "RAID stride                      : %u\n", sb->s_raid_stride);
    fprintf(stdout, "MMP checking wait time (seconds) : %u\n", sb->s_mmp_interval);
    fprintf(stdout, "MMP blocks                       : %llu\n", sb->s_mmp_block);
    fprintf(stdout, "RAID stripe width                : %u\n", sb->s_raid_stripe_width);

    val = (uint32_t)pow((double)2, (double)sb->s_log_groups_per_flex);
    fprintf(stdout, "Flexible block size              : %u\n", val);
    fprintf(stdout, "Reserved char padding 2          : %u\n", sb->s_reserved_char_pad2);
    fprintf(stdout, "Reserved padding                 : %u\n", sb->s_reserved_pad);
    fprintf(stdout, "KiB writtten                     : %llu\n", sb->s_kbytes_written);
  }

  fprintf(stdout, "\n");
}

void ext4_show_gdp_stat(struct ext4_super_block *sb, ext4_group_t bg, struct ext4_group_desc *gdp)
{
  if (sb->s_feature_incompat & EXT4_FEATURE_INCOMPAT_64BIT
      && sb->s_desc_size > EXT4_MIN_DESC_SIZE) {
    /*
     * Disused here due to 'struct ext4_group_desc_min' used here,
     * instead of 'struct ext4_group_desc'
     */
#if 0
    fprintf(stdout, "Group %2d: ", bg);
    fprintf(stdout, "block bitmap at %llu, ", ((__le64)gdp->bg_block_bitmap_hi << 32) | (__le64)gdp->bg_block_bitmap_lo);
    fprintf(stdout, "inode bitmap at %llu, ", ((__le64)gdp->bg_inode_bitmap_hi << 32) | (__le64)gdp->bg_inode_bitmap_lo);
    fprintf(stdout, "inode table at %llu, ", ((__le64)gdp->bg_inode_table_hi << 32) | (__le64)gdp->bg_inode_table_lo);

    fprintf(stdout, "\n          ");

    fprintf(stdout, "%llu free blocks, ", ((__le64)gdp->bg_free_blocks_count_hi << 32) | (__le64)gdp->bg_free_blocks_count_lo);
    fprintf(stdout, "%llu free inodes, ", ((__le64)gdp->bg_free_inodes_count_hi << 32) | (__le64)gdp->bg_free_inodes_count_lo);
    fprintf(stdout, "%llu used directories\n", ((__le64)gdp->bg_used_dirs_count_hi << 32) | (__le64)gdp->bg_used_dirs_count_lo);
#endif
  } else {
    fprintf(stdout, "Group %2d: ", bg);
    fprintf(stdout, "block bitmap at %u, ", (__le32)gdp->bg_block_bitmap_lo);
    fprintf(stdout, "inode bitmap at %u, ", (__le32)gdp->bg_inode_bitmap_lo);
    fprintf(stdout, "inode table at %u, ", (__le32)gdp->bg_inode_table_lo);

    fprintf(stdout, "\n          ");

    fprintf(stdout, "%u free blocks, ", (__le16)gdp->bg_free_blocks_count_lo);
    fprintf(stdout, "%u free inodes, ", (__le16)gdp->bg_free_inodes_count_lo);
    fprintf(stdout, "%u used directories\n", (__le16)gdp->bg_used_dirs_count_lo);
  }

  fprintf(stdout, "\n");
}

void ext4_show_inode_stat(struct ext4_super_block *sb, uint64_t ino, struct ext4_inode *inode)
{
  const char *str = NULL;
  time_t tm = 0;

  fprintf(stdout, "Inode %5lu: ", ino);

  str = NULL;
  fprintf(stdout, "type: ");
  if (inode->i_mode & EXT4_INODE_MODE_S_IFIFO) {
    str = "FIFO";
  } else if (inode->i_mode & EXT4_INODE_MODE_S_IFCHR) {
    str = "character device";
  } else if (inode->i_mode & EXT4_INODE_MODE_S_IFDIR) {
    str = "directory";
  } else if (inode->i_mode & EXT4_INODE_MODE_S_IFBLK) {
    str = "block device";
  } else if (inode->i_mode & EXT4_INODE_MODE_S_IFREG) {
    str = "regular file";
  } else if (inode->i_mode & EXT4_INODE_MODE_S_IFLNK) {
    str = "symbolic link";
  } else if (inode->i_mode & EXT4_INODE_MODE_S_IFSOCK) {
    str = "socket";
  } else if (inode->i_mode & EXT4_INODE_MODE_S_IFCHR) {
    str = EXT4_DUMMY_STR;
  }
  fprintf(stdout, "%s  ", str);

  fprintf(stdout, "mode: %04o  ", inode->i_mode & 0777);

  fprintf(stdout, "\n             ");

  str = NULL;
  fprintf(stdout, "flags: ");
  if (inode->i_flags & EXT4_SECRM_FL) {
    str = "file requires secure deletion";
    fprintf(stdout, "%s, ", str);
  }
  if (inode->i_flags & EXT4_UNRM_FL) {
    str = "file should be preserved and undeleted";
    fprintf(stdout, "%s, ", str);
  }
  if (inode->i_flags & EXT4_COMPR_FL) {
    str = "file is compressed";
    fprintf(stdout, "%s, ", str);
  }
  if (inode->i_flags & EXT4_SYNC_FL) {
    str = "all writes to the file must be synchronous";
    fprintf(stdout, "%s, ", str);
  }
  if (inode->i_flags & EXT4_IMMUTABLE_FL) {
    str = "file is immutable";
    fprintf(stdout, "%s, ", str);
  }
  if (inode->i_flags & EXT4_APPEND_FL) {
    str = "file can only be appended";
    fprintf(stdout, "%s, ", str);
  }
  if (inode->i_flags & EXT4_NODUMP_FL) {
    str = "dump utility should not dump this file";
    fprintf(stdout, "%s, ", str);
  }
  if (inode->i_flags & EXT4_NOATIME_FL) {
    str = "do not update access time";
    fprintf(stdout, "%s, ", str);
  }
  if (inode->i_flags & EXT4_DIRTY_FL) {
    str = "dirty compressed file";
    fprintf(stdout, "%s, ", str);
  }
  if (inode->i_flags & EXT4_COMPRBLK_FL) {
    str = "file has one or more compressed clusters";
    fprintf(stdout, "%s, ", str);
  }
  if (inode->i_flags & EXT4_NOCOMPR_FL) {
    str = "do not compress file";
    fprintf(stdout, "%s, ", str);
  }
  if (inode->i_flags & EXT4_ECOMPR_FL) {
    str = "compression error";
    fprintf(stdout, "%s, ", str);
  }
  if (inode->i_flags & EXT4_INDEX_FL) {
    str = "directory has hashed indexes";
    fprintf(stdout, "%s, ", str);
  }
  if (inode->i_flags & EXT4_IMAGIC_FL) {
    str = "AFS magic directory";
    fprintf(stdout, "%s, ", str);
  }
  if (inode->i_flags & EXT4_JOURNAL_DATA_FL) {
    str = "file data must always be written through the journal";
    fprintf(stdout, "%s, ", str);
  }
  if (inode->i_flags & EXT4_NOTAIL_FL) {
    str = "file tail should not be merged";
    fprintf(stdout, "%s, ", str);
  }
  if (inode->i_flags & EXT4_DIRSYNC_FL) {
    str = "all directory entry data should be written synchronously";
    fprintf(stdout, "%s, ", str);
  }
  if (inode->i_flags & EXT4_TOPDIR_FL) {
    str = "top of directory hierarchy";
    fprintf(stdout, "%s, ", str);
  }
  if (inode->i_flags & EXT4_HUGE_FILE_FL) {
    str = "huge file";
    fprintf(stdout, "%s, ", str);
  }
  if (inode->i_flags & EXT4_EXTENTS_FL) {
    str = "inode uses extents";
    fprintf(stdout, "%s, ", str);
  }
  if (inode->i_flags & EXT4_EA_INODE_FL) {
    str = "inode used for a large extended attribute";
    fprintf(stdout, "%s, ", str);
  }
  if (inode->i_flags & EXT4_EOFBLOCKS_FL) {
    str = "file has blocks allocated past EOF";
    fprintf(stdout, "%s, ", str);
  }
  if (inode->i_flags & EXT4_RESERVED_FL) {
    str = "reserved for ext4 library";
    fprintf(stdout, "%s, ", str);
  }
  if (inode->i_flags & EXT4_FL_USER_VISIBLE) {
    str = "user-visible flags";
    fprintf(stdout, "%s, ", str);
  }
  if (inode->i_flags & EXT4_FL_USER_MODIFIABLE) {
    str = "user-modifiable flags";
    fprintf(stdout, "%s, ", str);
  }
  if (str == NULL) {
    fprintf(stdout, EXT4_DUMMY_STR);
  }

  fprintf(stdout, "\n             ");

  fprintf(stdout, "generation: %u  ", inode->i_generation);

  if (sb->s_inode_size > EXT4_GOOD_OLD_INODE_SIZE && inode->i_extra_isize >= 24) {
    fprintf(stdout, "version: 0x%08x:%08x", inode->i_version_hi, inode->osd1.linux1.l_i_version);
  } else {
    fprintf(stdout, "version: 0x%08x", inode->osd1.linux1.l_i_version);
  }

  fprintf(stdout, "\n             ");

  fprintf(stdout, "user: %u  ", inode->i_uid);
  fprintf(stdout, "group: %u  ", inode->i_gid);
  fprintf(stdout, "size: %llu", ((__le64)inode->i_size_high << 32) | (__le64)inode->i_size_lo);

  fprintf(stdout, "\n             ");

  fprintf(stdout, "file ACL: %llu", ((__le64)inode->osd2.linux2.l_i_file_acl_high << 32) | (__le64)inode->i_file_acl_lo);

  fprintf(stdout, "\n             ");

  fprintf(stdout, "link count: %u  ", inode->i_links_count);
  fprintf(stdout, "block count: %llu", ((__le64)inode->osd2.linux2.l_i_blocks_high << 32) | (__le64)inode->i_blocks_lo);

  fprintf(stdout, "\n             ");

  fprintf(stdout, "ctime: ");
  if (inode->i_ctime != 0) {
    tm = (time_t)inode->i_ctime;
    fprintf(stdout, "%s", ctime(&tm));
    fprintf(stdout, "             ");
  } else {
    fprintf(stdout, EXT4_DUMMY_STR);
    fprintf(stdout, "\n             ");
  }

  fprintf(stdout, "atime: ");
  if (inode->i_atime != 0) {
    tm = (time_t)inode->i_atime;
    fprintf(stdout, "%s", ctime(&tm));
    fprintf(stdout, "             ");
  } else {
    fprintf(stdout, EXT4_DUMMY_STR);
    fprintf(stdout, "\n             ");
  }

  fprintf(stdout, "mtime: ");
  if (inode->i_mtime != 0) {
    tm = (time_t)inode->i_mtime;
    fprintf(stdout, "%s", ctime(&tm));
    fprintf(stdout, "             ");
  } else {
    fprintf(stdout, EXT4_DUMMY_STR);
    fprintf(stdout, "\n             ");
  }

  fprintf(stdout, "size of extra inode: %u", inode->i_extra_isize);

  fprintf(stdout, "\n             ");

  fprintf(stdout, "extents: %s\n", EXT4_DUMMY_STR);

  fprintf(stdout, "\n");
}

void ext4_show_extent_header(struct ext4_extent_header *eh)
{
  fprintf(stdout, "Extent header: ");

  fprintf(stdout, "magic: 0x%X  ", eh->eh_magic);

  fprintf(stdout, "\n               ");

  fprintf(stdout, "valid entries: %u  ", eh->eh_entries);
  fprintf(stdout, "max entries: %u", eh->eh_max);

  fprintf(stdout, "\n               ");

  fprintf(stdout, "depth: %u", eh->eh_depth);

  fprintf(stdout, "\n               ");

  fprintf(stdout, "generation: %u\n", eh->eh_generation);

  fprintf(stdout, "\n");
}

void ext4_show_extent_idx(struct ext4_extent_idx *ei)
{
  fprintf(stdout, "Extent internal node: ");

  fprintf(stdout, "file blocks covered: %u  ", ei->ei_block);
  fprintf(stdout, "block pointed to: %llu\n", ((__le64)ei->ei_leaf_hi << 32) | (__le64)ei->ei_leaf_lo);

  fprintf(stdout, "\n");
}

void ext4_show_extent(struct ext4_extent *ext)
{
  fprintf(stdout, "Extent leaf node: ");

  fprintf(stdout, "first file block: %u", ext->ee_block);

  fprintf(stdout, "\n                  ");

  fprintf(stdout, "blocks: %u", ext->ee_len);

  fprintf(stdout, "\n                  ");

  fprintf(stdout, "block pointed to: %llu\n", ((__le64)ext->ee_start_hi << 32) | (__le64)ext->ee_start_lo);

  fprintf(stdout, "\n");
}

void ext4_show_dentry(struct ext4_dir_entry_2 *dentry)
{
  int32_t i = 0;
  const char *str = NULL;

  fprintf(stdout, "Linear dentry: ");

  fprintf(stdout, "inode: %u", dentry->inode);

  fprintf(stdout, "\n               ");

  fprintf(stdout, "entry length: %u", dentry->rec_len);

  fprintf(stdout, "\n               ");

  fprintf(stdout, "name length: %u", dentry->name_len);

  fprintf(stdout, "\n               ");

  switch (dentry->file_type) {
  case EXT4_FT_UNKNOWN:
    str = "unknown";
    break;
  case EXT4_FT_REG_FILE:
    str = "regular file";
    break;
  case EXT4_FT_DIR:
    str = "directory";
    break;
  case EXT4_FT_CHRDEV:
    str = "character device file";
    break;
  case EXT4_FT_BLKDEV:
    str = "block device file";
    break;
  case EXT4_FT_FIFO:
    str = "FIFO";
    break;
  case EXT4_FT_SOCK:
    str = "socket";
    break;
  case EXT4_FT_SYMLINK:
    str = "symbolic link";
    break;
  default:
    str = EXT4_DUMMY_STR;
    break;
  }
  fprintf(stdout, "type: %s", str);

  fprintf(stdout, "\n               ");

  fprintf(stdout, "name: ");
  for (i = 0; i < dentry->name_len; ++i) {
    fprintf(stdout, "%c", dentry->name[i]);
  }

  fprintf(stdout, "\n\n");
}
