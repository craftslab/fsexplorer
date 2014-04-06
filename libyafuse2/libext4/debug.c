/**
 * debug.c - debugging Ext4.
 *
 * Copyright (c) 2013-2014 angersax@gmail.com
 *
 * This file is part of libyafuse2.
 *
 * libyafuse2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libyafuse2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libyafuse2.  If not, see <http://www.gnu.org/licenses/>.
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
void ext4_show_stat_sb(struct ext4_super_block *sb, char *buf, int32_t buf_len)
{
  int32_t i = 0;
  uint32_t val = 0;
  const char *str = NULL;
  time_t tm = 0;
  int32_t len = 0;

  len = snprintf(buf, buf_len, "Total inode count : %u\n", sb->s_inodes_count);
  buf += len;
  len = snprintf(buf, buf_len, "Total block count : %llu\n", ((__le64)sb->s_blocks_count_hi << 32) | (__le64)sb->s_blocks_count_lo);
  buf += len;
  len = snprintf(buf, buf_len, "Reserved block count : %llu\n", ((__le64)sb->s_r_blocks_count_hi << 32) | (__le64)sb->s_r_blocks_count_lo);
  buf += len;
  len = snprintf(buf, buf_len, "Free block count : %llu\n", ((__le64)sb->s_free_blocks_count_hi << 32) | (__le64)sb->s_free_blocks_count_lo);
  buf += len;
  len = snprintf(buf, buf_len, "Free inode count : %u\n", sb->s_free_inodes_count);
  buf += len;
  len = snprintf(buf, buf_len, "First data block : %u\n", sb->s_first_data_block);
  buf += len;

  val = (uint32_t)pow((double)2, (double)(10 + sb->s_log_block_size));
  len = snprintf(buf, buf_len, "Block size : %u\n", val);
  buf += len;

  val = (uint32_t)pow((double)2, (double)(10 + sb->s_obso_log_frag_size));
  len = snprintf(buf, buf_len, "Fragment size (obsolete) : %u\n", val);
  buf += len;

  len = snprintf(buf, buf_len, "Blocks per group : %u\n", sb->s_blocks_per_group);
  buf += len;
  len = snprintf(buf, buf_len, "Fragments per group (obsolete) : %u\n", sb->s_obso_frags_per_group);
  buf += len;
  len = snprintf(buf, buf_len, "Inodes per group : %u\n", sb->s_inodes_per_group);
  buf += len;

  len = snprintf(buf, buf_len, "Mount time : ");
  buf += len;
  if (sb->s_mtime != 0) {
    tm = (time_t)sb->s_mtime;
    len = snprintf(buf, buf_len, "%s", ctime(&tm));
    buf += len;
  } else {
    len = snprintf(buf, buf_len, EXT4_DUMMY_STR);
    buf += len;
    len = snprintf(buf, buf_len, "\n");
    buf += len;
  }

  len = snprintf(buf, buf_len, "Write time : ");
  buf += len;
  if (sb->s_wtime != 0) {
    tm = (time_t)sb->s_wtime;
    len = snprintf(buf, buf_len, "%s", ctime(&tm));
    buf += len;
  } else {
    len = snprintf(buf, buf_len, EXT4_DUMMY_STR);
    buf += len;
    len = snprintf(buf, buf_len, "\n");
    buf += len;
  }

  len = snprintf(buf, buf_len, "Mount count : %u\n", sb->s_mnt_count);
  buf += len;
  len = snprintf(buf, buf_len, "Maximum mount count : %u\n", sb->s_max_mnt_count);
  buf += len;
  len = snprintf(buf, buf_len, "Magic signature : 0x%X\n", sb->s_magic);
  buf += len;

  len = snprintf(buf, buf_len, "File system state : ");
  buf += len;
  str = NULL;
  if (sb->s_state & EXT4_VALID_FS) {
    str = "cleanly umounted";
    len = snprintf(buf, buf_len, "%s ", str);
    buf += len;
  }
  if (sb->s_state & EXT4_ERROR_FS) {
    str = "errors detected";
    len = snprintf(buf, buf_len, "%s ", str);
    buf += len;
  }
  if (sb->s_state & EXT4_ORPHAN_FS) {
    str = "orphans being recovered";
    len = snprintf(buf, buf_len, "%s ", str);
    buf += len;
  }
  if (!str) {
    str = EXT4_DUMMY_STR;
    len = snprintf(buf, buf_len, "%s", str);
    buf += len;
  }
  len = snprintf(buf, buf_len, "\n");
  buf += len;

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
  len = snprintf(buf, buf_len, "Errors behaviour : %s\n", str);
  buf += len;

  len = snprintf(buf, buf_len, "Minor revision level : %u\n", sb->s_minor_rev_level);
  buf += len;
  len = snprintf(buf, buf_len, "Last checked : %u\n", sb->s_lastcheck);
  buf += len;
  len = snprintf(buf, buf_len, "Check interval : %u\n", sb->s_checkinterval);
  buf += len;

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
  len = snprintf(buf, buf_len, "OS type : %s\n", str);
  buf += len;

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
  len = snprintf(buf, buf_len, "Revision level : %s\n", str);
  buf += len;

  len = snprintf(buf, buf_len, "Reserved blocks uid : %u\n", sb->s_def_resuid);
  buf += len;
  len = snprintf(buf, buf_len, "Reserved blocks gid : %u\n", sb->s_def_resgid);
  buf += len;

  if (sb->s_rev_level == EXT4_DYNAMIC_REV) {
    len = snprintf(buf, buf_len, "\n");
    buf += len;
    len = snprintf(buf, buf_len, "First non-reserved inode : %u\n", sb->s_first_ino);
    buf += len;
    len = snprintf(buf, buf_len, "Inode size : %u\n", sb->s_inode_size);
    buf += len;
    len = snprintf(buf, buf_len, "Block group number : %u\n", sb->s_block_group_nr);
    buf += len;

    str = NULL;
    len = snprintf(buf, buf_len, "Compatible feature : ");
    buf += len;
    if (sb->s_feature_compat & EXT4_FEATURE_COMPAT_DIR_PREALLOC) {
      str = "directory preallocation";
      len = snprintf(buf, buf_len, "%s, ", str);
      buf += len;
    }
    if (sb->s_feature_compat & EXT4_FEATURE_COMPAT_IMAGIC_INODES) {
      str = "imagic inodes";
      len = snprintf(buf, buf_len, "%s, ", str);
      buf += len;
    }
    if (sb->s_feature_compat & EXT4_FEATURE_COMPAT_HAS_JOURNAL) {
      str = "has a journal";
      len = snprintf(buf, buf_len, "%s, ", str);
      buf += len;
    }
    if (sb->s_feature_compat & EXT4_FEATURE_COMPAT_EXT_ATTR) {
      str = "support extended attributes";
      len = snprintf(buf, buf_len, "%s, ", str);
      buf += len;
    }
    if (sb->s_feature_compat & EXT4_FEATURE_COMPAT_RESIZE_INODE) {
      str = "has reserved GDT blocks for filesystem expansion";
      len = snprintf(buf, buf_len, "%s, ", str);
      buf += len;
    }
    if (sb->s_feature_compat & EXT4_FEATURE_COMPAT_DIR_INDEX) {
      str = "has directory indices";
      len = snprintf(buf, buf_len, "%s, ", str);
      buf += len;
    }
    if (sb->s_feature_compat & 0x40) {
      str = "lazy BG";
      len = snprintf(buf, buf_len, "%s, ", str);
      buf += len;
    }
    if (sb->s_feature_compat & 0x80) {
      str = "exclude inode";
      len = snprintf(buf, buf_len, "%s, ", str);
      buf += len;
    }
    if (str == NULL) {
      len = snprintf(buf, buf_len, EXT4_DUMMY_STR);
      buf += len;
    }
    len = snprintf(buf, buf_len, "\n");
    buf += len;

    str = NULL;
    len = snprintf(buf, buf_len, "Incompatible feature : ");
    buf += len;
    if (sb->s_feature_incompat & EXT4_FEATURE_INCOMPAT_COMPRESSION) {
      str = "compression";
      len = snprintf(buf, buf_len, "%s, ", str);
      buf += len;
    }
    if (sb->s_feature_incompat & EXT4_FEATURE_INCOMPAT_FILETYPE) {
      str = "directory entries record the file type";
      len = snprintf(buf, buf_len, "%s, ", str);
      buf += len;
    }
    if (sb->s_feature_incompat & EXT4_FEATURE_INCOMPAT_RECOVER) {
      str = "need recovery";
      len = snprintf(buf, buf_len, "%s, ", str);
      buf += len;
    }
    if (sb->s_feature_incompat & EXT4_FEATURE_INCOMPAT_JOURNAL_DEV) {
      str = "has a separate journal device";
      len = snprintf(buf, buf_len, "%s, ", str);
      buf += len;
    }
    if (sb->s_feature_incompat & EXT4_FEATURE_INCOMPAT_META_BG) {
      str = "meta block groups";
      len = snprintf(buf, buf_len, "%s, ", str);
      buf += len;
    }
    if (sb->s_feature_incompat & EXT4_FEATURE_INCOMPAT_EXTENTS) {
      str = "use extents for file";
      len = snprintf(buf, buf_len, "%s, ", str);
      buf += len;
    }
    if (sb->s_feature_incompat & EXT4_FEATURE_INCOMPAT_64BIT) {
      str = "support size of 2^64 blocks";
      len = snprintf(buf, buf_len, "%s, ", str);
      buf += len;
    }
    if (sb->s_feature_incompat & EXT4_FEATURE_INCOMPAT_MMP) {
      str = "multiple mount protection";
      len = snprintf(buf, buf_len, "%s, ", str);
      buf += len;
    }
    if (sb->s_feature_incompat & EXT4_FEATURE_INCOMPAT_FLEX_BG) {
      str = "flexible block groups";
      len = snprintf(buf, buf_len, "%s, ", str);
      buf += len;
    }
    if (sb->s_feature_incompat & EXT4_FEATURE_INCOMPAT_EA_INODE) {
      str = "support inode with large extended attributes";
      len = snprintf(buf, buf_len, "%s, ", str);
      buf += len;
    }
    if (sb->s_feature_incompat & EXT4_FEATURE_INCOMPAT_DIRDATA) {
      str = "data in directory entry";
      len = snprintf(buf, buf_len, "%s, ", str);
      buf += len;
    }
    if (str == NULL) {
      len = snprintf(buf, buf_len, EXT4_DUMMY_STR);
      buf += len;
    }
    len = snprintf(buf, buf_len, "\n");
    buf += len;

    str = NULL;
    len = snprintf(buf, buf_len, "Readonly-compatible feature : ");
    buf += len;
    if (sb->s_feature_ro_compat & EXT4_FEATURE_RO_COMPAT_SPARSE_SUPER) {
      str = "sparse superblocks";
      len = snprintf(buf, buf_len, "%s, ", str);
      buf += len;
    }
    if (sb->s_feature_ro_compat & EXT4_FEATURE_RO_COMPAT_LARGE_FILE) {
      str = "support file greater than 2GiB";
      len = snprintf(buf, buf_len, "%s, ", str);
      buf += len;
    }
    if (sb->s_feature_ro_compat & EXT4_FEATURE_RO_COMPAT_BTREE_DIR) {
      str = "has btree directory";
      len = snprintf(buf, buf_len, "%s, ", str);
      buf += len;
    }
    if (sb->s_feature_ro_compat & EXT4_FEATURE_RO_COMPAT_HUGE_FILE) {
      str = "support huge file";
      len = snprintf(buf, buf_len, "%s, ", str);
      buf += len;
    }
    if (sb->s_feature_ro_compat & EXT4_FEATURE_RO_COMPAT_GDT_CSUM) {
      str = "group descriptors have checksums";
      len = snprintf(buf, buf_len, "%s, ", str);
      buf += len;
    }
    if (sb->s_feature_ro_compat & EXT4_FEATURE_RO_COMPAT_DIR_NLINK) {
      str = "not support old ext3 32,000 subdirectory limit";
      len = snprintf(buf, buf_len, "%s, ", str);
      buf += len;
    }
    if (sb->s_feature_ro_compat & EXT4_FEATURE_RO_COMPAT_EXTRA_ISIZE) {
      str = "support large inodes";
      len = snprintf(buf, buf_len, "%s, ", str);
      buf += len;
    }
    if (sb->s_feature_ro_compat & 0x80) {
      str = "has a snapshot";
      len = snprintf(buf, buf_len, "%s, ", str);
      buf += len;
    }
    if (str == NULL) {
      len = snprintf(buf, buf_len, EXT4_DUMMY_STR);
      buf += len;
    }
    len = snprintf(buf, buf_len, "\n");
    buf += len;

    len = snprintf(buf, buf_len, "UUID : ");
    buf += len;
    for (i = 0; i < 16; ++i) {
      len = snprintf(buf, buf_len, "%x", sb->s_uuid[i]);
      buf += len;
    }
    len = snprintf(buf, buf_len, "\n");
    buf += len;

    len = snprintf(buf, buf_len, "Volume name : ");
    buf += len;
    if (sb->s_volume_name[0] == '\0') {
      len = snprintf(buf, buf_len, EXT4_DUMMY_STR);
      buf += len;
    } else {
      for (i = 0; i < 16; ++i) {
        len = snprintf(buf, buf_len, "%c", sb->s_volume_name[i]);
        buf += len;
      }
    }
    len = snprintf(buf, buf_len, "\n");
    buf += len;

    len = snprintf(buf, buf_len, "Last mounted on : ");
    buf += len;
    if (sb->s_last_mounted[0] == '\0') {
      len = snprintf(buf, buf_len, EXT4_DUMMY_STR);
      buf += len;
    } else {
      len = snprintf(buf, buf_len, "%s", sb->s_last_mounted);
      buf += len;
    }
    len = snprintf(buf, buf_len, "\n");
    buf += len;

    len = snprintf(buf, buf_len, "Bitmap algorithm usage : %u\n", sb->s_algorithm_usage_bitmap);
    buf += len;
  }

  if (sb->s_feature_compat & EXT4_FEATURE_COMPAT_DIR_PREALLOC) {
    len = snprintf(buf, buf_len, "\n");
    buf += len;
    len = snprintf(buf, buf_len, "Blocks preallocated for files : %u\n", sb->s_prealloc_blocks);
    buf += len;
    len = snprintf(buf, buf_len, "Blocks preallocated for dirs : %u\n", sb->s_prealloc_dir_blocks);
    buf += len;
    len = snprintf(buf, buf_len, "Reserved GDT blocks : %u\n", sb->s_reserved_gdt_blocks);
    buf += len;
  }

  if (sb->s_feature_compat & EXT4_FEATURE_COMPAT_HAS_JOURNAL) {
    len = snprintf(buf, buf_len, "\n");
    buf += len;
    len = snprintf(buf, buf_len, "Journal UUID : ");
    buf += len;
    for (i = 0; i < 16; ++i) {
      len = snprintf(buf, buf_len, "%x", sb->s_journal_uuid[i]);
      buf += len;
    }
    len = snprintf(buf, buf_len, "\n");
    buf += len;

    len = snprintf(buf, buf_len, "Journal inode : %u\n", sb->s_journal_inum);
    buf += len;
    len = snprintf(buf, buf_len, "Journal device : %u\n", sb->s_journal_dev);
    buf += len;
    len = snprintf(buf, buf_len, "Orphaned inodes to delete : %u\n", sb->s_last_orphan);
    buf += len;

    len = snprintf(buf, buf_len, "HTREE hash seed : ");
    buf += len;
    for (i = 0; i < (4 * 4); ++i) {
      len = snprintf(buf, buf_len, "%x", *((const __u8 *)(sb->s_hash_seed) + i));
      buf += len;
    }
    len = snprintf(buf, buf_len, "\n");
    buf += len;

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
    len = snprintf(buf, buf_len, "Default hash version for dirs hashes : %s\n", str);
    buf += len;

    len = snprintf(buf, buf_len, "Reserved char padding : %u\n", sb->s_reserved_char_pad);
    buf += len;
    len = snprintf(buf, buf_len, "Group descriptors size : %u\n", sb->s_desc_size);
    buf += len;

    str = NULL;
    len = snprintf(buf, buf_len, "Default mount options : ");
    buf += len;
    if (sb->s_default_mount_opts & EXT4_DEFM_DEBUG) {
      str = "print debugging info upon (re)mount";
      len = snprintf(buf, buf_len, "%s, ", str);
      buf += len;
    }
    if (sb->s_default_mount_opts & EXT4_DEFM_BSDGROUPS) {
      str = "New files take the gid of the containing directory";
      len = snprintf(buf, buf_len, "%s, ", str);
      buf += len;
    }
    if (sb->s_default_mount_opts & EXT4_DEFM_XATTR_USER) {
      str = "support userspace-provided extended attributes";
      len = snprintf(buf, buf_len, "%s, ", str);
      buf += len;
    }
    if (sb->s_default_mount_opts & EXT4_DEFM_ACL) {
      str = "support POSIX access control lists (ACLs)";
      len = snprintf(buf, buf_len, "%s, ", str);
      buf += len;
    }
    if (sb->s_default_mount_opts & EXT4_DEFM_UID16) {
      str = "not support 32-bit UIDs";
      len = snprintf(buf, buf_len, "%s, ", str);
      buf += len;
    }
    if (sb->s_default_mount_opts & EXT4_DEFM_JMODE_DATA) {
      str = "all data and metadata are commited to the journal";
      len = snprintf(buf, buf_len, "%s, ", str);
      buf += len;
    }
    if (sb->s_default_mount_opts & EXT4_DEFM_JMODE_ORDERED) {
      str = "all data are flushed to the disk before metadata are committed to the journal";
      len = snprintf(buf, buf_len, "%s, ", str);
      buf += len;
    }
    if (sb->s_default_mount_opts & EXT4_DEFM_JMODE_WBACK) {
      str = "data ordering is not preserved which may be written after the metadata has been written";
      len = snprintf(buf, buf_len, "%s, ", str);
      buf += len;
    }
    if (sb->s_default_mount_opts & EXT4_DEFM_NOBARRIER) {
      str = "disable write flushes";
      len = snprintf(buf, buf_len, "%s, ", str);
      buf += len;
    }
    if (sb->s_default_mount_opts & EXT4_DEFM_BLOCK_VALIDITY) {
      str = "track which blocks in a filesystem are metadata";
      len = snprintf(buf, buf_len, "%s, ", str);
      buf += len;
    }
    if (sb->s_default_mount_opts & EXT4_DEFM_DISCARD) {
      str = "enable DISCARD support";
      len = snprintf(buf, buf_len, "%s, ", str);
      buf += len;
    }
    if (sb->s_default_mount_opts & EXT4_DEFM_NODELALLOC) {
      str = "disable delayed allocation";
      len = snprintf(buf, buf_len, "%s, ", str);
      buf += len;
    }
    if (str == NULL) {
      len = snprintf(buf, buf_len, EXT4_DUMMY_STR);
      buf += len;
    }
    len = snprintf(buf, buf_len, "\n");
    buf += len;

    len = snprintf(buf, buf_len, "First metablock block group : %u\n", sb->s_first_meta_bg);
    buf += len;

    len = snprintf(buf, buf_len, "Filesystem-created time : ");
    buf += len;
    if (sb->s_mkfs_time != 0) {
      tm = (time_t)sb->s_mkfs_time;
      len = snprintf(buf, buf_len, "%s", ctime(&tm));
      buf += len;
    } else {
      len = snprintf(buf, buf_len, EXT4_DUMMY_STR);
      buf += len;
    }
    len = snprintf(buf, buf_len, "\n");
    buf += len;

    len = snprintf(buf, buf_len, "Journal backup : ");
    buf += len;
    for (i = 0; i < (17 * 4); ++i) {
      len = snprintf(buf, buf_len, "%x", *((const __u8 *)(sb->s_jnl_blocks) + i));
      buf += len;
    }
    len = snprintf(buf, buf_len, "\n");
    buf += len;
  }

  if (sb->s_feature_incompat & EXT4_FEATURE_INCOMPAT_64BIT) {
    len = snprintf(buf, buf_len, "\n");
    buf += len;
    len = snprintf(buf, buf_len, "Required extra isize : %u\n", sb->s_min_extra_isize);
    buf += len;
    len = snprintf(buf, buf_len, "Desired extra isize : %u\n", sb->s_want_extra_isize);
    buf += len;

    str = NULL;
    len = snprintf(buf, buf_len, "Misc flags : ");
    buf += len;
    if (sb->s_flags & EXT2_FLAGS_SIGNED_HASH) {
      str = "signed directory hash in use";
      len = snprintf(buf, buf_len, "%s, ", str);
      buf += len;
    }
    if (sb->s_flags & EXT2_FLAGS_UNSIGNED_HASH) {
      str = "unsigned directory hash in use";
      len = snprintf(buf, buf_len, "%s, ", str);
      buf += len;
    }
    if (sb->s_flags & EXT2_FLAGS_TEST_FILESYS) {
      str = "to test development code";
      len = snprintf(buf, buf_len, "%s, ", str);
      buf += len;
    }
    if (sb->s_flags & 0x10) {
      str = "is snapshot";
      len = snprintf(buf, buf_len, "%s, ", str);
      buf += len;
    }
    if (sb->s_flags & 0x20) {
      str = "fix snapshot";
      len = snprintf(buf, buf_len, "%s, ", str);
      buf += len;
    }
    if (sb->s_flags & 0x40) {
      str = "fix exclusion";
      len = snprintf(buf, buf_len, "%s, ", str);
      buf += len;
    }
    if (str == NULL) {
      len = snprintf(buf, buf_len, EXT4_DUMMY_STR);
      buf += len;
    }
    len = snprintf(buf, buf_len, "\n");
    buf += len;

    len = snprintf(buf, buf_len, "RAID stride : %u\n", sb->s_raid_stride);
    buf += len;
    len = snprintf(buf, buf_len, "MMP checking wait time (seconds) : %u\n", sb->s_mmp_interval);
    buf += len;
    len = snprintf(buf, buf_len, "MMP blocks : %llu\n", sb->s_mmp_block);
    buf += len;
    len = snprintf(buf, buf_len, "RAID stripe width : %u\n", sb->s_raid_stripe_width);
    buf += len;

    val = (uint32_t)pow((double)2, (double)sb->s_log_groups_per_flex);
    len = snprintf(buf, buf_len, "Flexible block size : %u\n", val);
    buf += len;
    len = snprintf(buf, buf_len, "Reserved char padding 2 : %u\n", sb->s_reserved_char_pad2);
    buf += len;
    len = snprintf(buf, buf_len, "Reserved padding : %u\n", sb->s_reserved_pad);
    buf += len;
    len = snprintf(buf, buf_len, "KiB writtten : %llu\n", sb->s_kbytes_written);
    buf += len;
  }

  len = snprintf(buf, buf_len, "\n");
  buf += len;
}

void ext4_show_stat_gdp(struct ext4_super_block *sb, ext4_group_t bg, struct ext4_group_desc *gdp, char *buf, int32_t buf_len)
{
  int32_t len = 0;

  if (sb->s_feature_incompat & EXT4_FEATURE_INCOMPAT_64BIT
      && sb->s_desc_size > EXT4_MIN_DESC_SIZE) {
    /*
     * Disused here due to 'struct ext4_group_desc_min' used here,
     * instead of 'struct ext4_group_desc'
     */
#if 0
    len = snprintf(buf, buf_len, "Group : %d\n", bg);
    buf += len;
    len = snprintf(buf, buf_len, "block bitmap at : %llu\n", ((__le64)gdp->bg_block_bitmap_hi << 32) | (__le64)gdp->bg_block_bitmap_lo);
    buf += len;
    len = snprintf(buf, buf_len, "inode bitmap at : %llu\n", ((__le64)gdp->bg_inode_bitmap_hi << 32) | (__le64)gdp->bg_inode_bitmap_lo);
    buf += len;
    len = snprintf(buf, buf_len, "inode table at : %llu\n", ((__le64)gdp->bg_inode_table_hi << 32) | (__le64)gdp->bg_inode_table_lo);
    buf += len;
    len = snprintf(buf, buf_len, "free blocks : %llu\n", ((__le64)gdp->bg_free_blocks_count_hi << 32) | (__le64)gdp->bg_free_blocks_count_lo);
    buf += len;
    len = snprintf(buf, buf_len, "free inodes : %llu\n", ((__le64)gdp->bg_free_inodes_count_hi << 32) | (__le64)gdp->bg_free_inodes_count_lo);
    buf += len;
    len = snprintf(buf, buf_len, "directories used : %llu\n", ((__le64)gdp->bg_used_dirs_count_hi << 32) | (__le64)gdp->bg_used_dirs_count_lo);
    buf += len;
#endif
  } else {
    len = snprintf(buf, buf_len, "Group : %d\n", bg);
    buf += len;
    len = snprintf(buf, buf_len, "block bitmap at : %u\n", (__le32)gdp->bg_block_bitmap_lo);
    buf += len;
    len = snprintf(buf, buf_len, "inode bitmap at : %u\n", (__le32)gdp->bg_inode_bitmap_lo);
    buf += len;
    len = snprintf(buf, buf_len, "inode table at : %u\n", (__le32)gdp->bg_inode_table_lo);
    buf += len;
    len = snprintf(buf, buf_len, "free blocks : %u\n", (__le16)gdp->bg_free_blocks_count_lo);
    buf += len;
    len = snprintf(buf, buf_len, "free inodes : %u\n", (__le16)gdp->bg_free_inodes_count_lo);
    buf += len;
    len = snprintf(buf, buf_len, "directories used : %u\n", (__le16)gdp->bg_used_dirs_count_lo);
    buf += len;
  }

  len = snprintf(buf, buf_len, "\n");
  buf += len;
}

void ext4_show_stat_inode(struct ext4_super_block *sb, uint64_t ino, struct ext4_inode *inode, char *buf, int32_t buf_len)
{
  const char *str = NULL;
  time_t tm = 0;
  int32_t len = 0;

  len = snprintf(buf, buf_len, "Inode : %llu\n", (long long unsigned)ino);
  buf += len;

  len = snprintf(buf, buf_len, "type : ");
  buf += len;
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
  } else {
    str = NULL;
  }
  len = snprintf(buf, buf_len, "%s\n", str);
  buf += len;

  len = snprintf(buf, buf_len, "mode : %04o\n", inode->i_mode & 0777);
  buf += len;

  str = NULL;
  len = snprintf(buf, buf_len, "flags : ");
  buf += len;
  if (inode->i_flags & EXT4_SECRM_FL) {
    str = "file requires secure deletion";
    len = snprintf(buf, buf_len, "%s, ", str);
    buf += len;
  }
  if (inode->i_flags & EXT4_UNRM_FL) {
    str = "file should be preserved and undeleted";
    len = snprintf(buf, buf_len, "%s, ", str);
    buf += len;
  }
  if (inode->i_flags & EXT4_COMPR_FL) {
    str = "file is compressed";
    len = snprintf(buf, buf_len, "%s, ", str);
    buf += len;
  }
  if (inode->i_flags & EXT4_SYNC_FL) {
    str = "all writes to the file must be synchronous";
    len = snprintf(buf, buf_len, "%s, ", str);
    buf += len;
  }
  if (inode->i_flags & EXT4_IMMUTABLE_FL) {
    str = "file is immutable";
    len = snprintf(buf, buf_len, "%s, ", str);
    buf += len;
  }
  if (inode->i_flags & EXT4_APPEND_FL) {
    str = "file can only be appended";
    len = snprintf(buf, buf_len, "%s, ", str);
    buf += len;
  }
  if (inode->i_flags & EXT4_NODUMP_FL) {
    str = "dump utility should not dump this file";
    len = snprintf(buf, buf_len, "%s, ", str);
    buf += len;
  }
  if (inode->i_flags & EXT4_NOATIME_FL) {
    str = "do not update access time";
    len = snprintf(buf, buf_len, "%s, ", str);
    buf += len;
  }
  if (inode->i_flags & EXT4_DIRTY_FL) {
    str = "dirty compressed file";
    len = snprintf(buf, buf_len, "%s, ", str);
    buf += len;
  }
  if (inode->i_flags & EXT4_COMPRBLK_FL) {
    str = "file has one or more compressed clusters";
    len = snprintf(buf, buf_len, "%s, ", str);
    buf += len;
  }
  if (inode->i_flags & EXT4_NOCOMPR_FL) {
    str = "do not compress file";
    len = snprintf(buf, buf_len, "%s, ", str);
    buf += len;
  }
  if (inode->i_flags & EXT4_ECOMPR_FL) {
    str = "compression error";
    len = snprintf(buf, buf_len, "%s, ", str);
    buf += len;
  }
  if (inode->i_flags & EXT4_INDEX_FL) {
    str = "directory has hashed indexes";
    len = snprintf(buf, buf_len, "%s, ", str);
    buf += len;
  }
  if (inode->i_flags & EXT4_IMAGIC_FL) {
    str = "AFS magic directory";
    len = snprintf(buf, buf_len, "%s, ", str);
    buf += len;
  }
  if (inode->i_flags & EXT4_JOURNAL_DATA_FL) {
    str = "file data must always be written through the journal";
    len = snprintf(buf, buf_len, "%s, ", str);
    buf += len;
  }
  if (inode->i_flags & EXT4_NOTAIL_FL) {
    str = "file tail should not be merged";
    len = snprintf(buf, buf_len, "%s, ", str);
    buf += len;
  }
  if (inode->i_flags & EXT4_DIRSYNC_FL) {
    str = "all directory entry data should be written synchronously";
    len = snprintf(buf, buf_len, "%s, ", str);
    buf += len;
  }
  if (inode->i_flags & EXT4_TOPDIR_FL) {
    str = "top of directory hierarchy";
    len = snprintf(buf, buf_len, "%s, ", str);
    buf += len;
  }
  if (inode->i_flags & EXT4_HUGE_FILE_FL) {
    str = "huge file";
    len = snprintf(buf, buf_len, "%s, ", str);
    buf += len;
  }
  if (inode->i_flags & EXT4_EXTENTS_FL) {
    str = "inode uses extents";
    len = snprintf(buf, buf_len, "%s, ", str);
    buf += len;
  }
  if (inode->i_flags & EXT4_EA_INODE_FL) {
    str = "inode used for a large extended attribute";
    len = snprintf(buf, buf_len, "%s, ", str);
    buf += len;
  }
  if (inode->i_flags & EXT4_EOFBLOCKS_FL) {
    str = "file has blocks allocated past EOF";
    len = snprintf(buf, buf_len, "%s, ", str);
    buf += len;
  }
  if (inode->i_flags & EXT4_RESERVED_FL) {
    str = "reserved for ext4 library";
    len = snprintf(buf, buf_len, "%s, ", str);
    buf += len;
  }
  if (inode->i_flags & EXT4_FL_USER_VISIBLE) {
    str = "user-visible flags";
    len = snprintf(buf, buf_len, "%s, ", str);
    buf += len;
  }
  if (inode->i_flags & EXT4_FL_USER_MODIFIABLE) {
    str = "user-modifiable flags";
    len = snprintf(buf, buf_len, "%s, ", str);
    buf += len;
  }
  if (str == NULL) {
    len = snprintf(buf, buf_len, EXT4_DUMMY_STR);
    buf += len;
  }
  len = snprintf(buf, buf_len, "\n");
  buf += len;

  len = snprintf(buf, buf_len, "generation : %u\n", inode->i_generation);
  buf += len;

  if (sb->s_inode_size > EXT4_GOOD_OLD_INODE_SIZE && inode->i_extra_isize >= 24) {
    len = snprintf(buf, buf_len, "version : 0x%08x:%08x", inode->i_version_hi, inode->osd1.linux1.l_i_version);
    buf += len;
  } else {
    len = snprintf(buf, buf_len, "version : 0x%08x", inode->osd1.linux1.l_i_version);
    buf += len;
  }
  len = snprintf(buf, buf_len, "\n");
  buf += len;

  len = snprintf(buf, buf_len, "user : %u\n", inode->i_uid);
  buf += len;
  len = snprintf(buf, buf_len, "group : %u\n", inode->i_gid);
  buf += len;
  len = snprintf(buf, buf_len, "size : %llu\n", ((__le64)inode->i_size_high << 32) | (__le64)inode->i_size_lo);
  buf += len;

  len = snprintf(buf, buf_len, "file ACL : %llu\n", ((__le64)inode->osd2.linux2.l_i_file_acl_high << 32) | (__le64)inode->i_file_acl_lo);
  buf += len;

  len = snprintf(buf, buf_len, "link count : %u\n", inode->i_links_count);
  buf += len;
  len = snprintf(buf, buf_len, "block count : %llu\n", ((__le64)inode->osd2.linux2.l_i_blocks_high << 32) | (__le64)inode->i_blocks_lo);
  buf += len;

  len = snprintf(buf, buf_len, "ctime : ");
  buf += len;
  if (inode->i_ctime != 0) {
    tm = (time_t)inode->i_ctime;
    len = snprintf(buf, buf_len, "%s", ctime(&tm));
    buf += len;
  } else {
    len = snprintf(buf, buf_len, "%s", EXT4_DUMMY_STR);
    buf += len;
  }

  len = snprintf(buf, buf_len, "atime : ");
  buf += len;
  if (inode->i_atime != 0) {
    tm = (time_t)inode->i_atime;
    len = snprintf(buf, buf_len, "%s", ctime(&tm));
    buf += len;
  } else {
    len = snprintf(buf, buf_len, "%s", EXT4_DUMMY_STR);
    buf += len;
  }

  len = snprintf(buf, buf_len, "mtime : ");
  buf += len;
  if (inode->i_mtime != 0) {
    tm = (time_t)inode->i_mtime;
    len = snprintf(buf, buf_len, "%s", ctime(&tm));
    buf += len;
  } else {
    len = snprintf(buf, buf_len, "%s", EXT4_DUMMY_STR);
    buf += len;
  }

  len = snprintf(buf, buf_len, "size of extra inode : %u\n", inode->i_extra_isize);
  buf += len;

  len = snprintf(buf, buf_len, "extents : %s\n", EXT4_DUMMY_STR);
  buf += len;

  len = snprintf(buf, buf_len, "\n");
  buf += len;
}

void ext4_show_stat_extent_header(struct ext4_extent_header *eh, char *buf, int32_t buf_len)
{
  int32_t len = 0;

  len = snprintf(buf, buf_len, "Extent header :\n");
  buf += len;
  len = snprintf(buf, buf_len, "magic : 0x%X\n", eh->eh_magic);
  buf += len;
  len = snprintf(buf, buf_len, "valid entries : %u\n", eh->eh_entries);
  buf += len;
  len = snprintf(buf, buf_len, "max entries : %u\n", eh->eh_max);
  buf += len;
  len = snprintf(buf, buf_len, "depth : %u\n", eh->eh_depth);
  buf += len;
  len = snprintf(buf, buf_len, "generation : %u\n", eh->eh_generation);
  buf += len;

  len = snprintf(buf, buf_len, "\n");
  buf += len;
}

void ext4_show_stat_extent_idx(struct ext4_extent_idx *ei, char *buf, int32_t buf_len)
{
  int32_t len = 0;

  len = snprintf(buf, buf_len, "Extent internal node :\n");
  buf += len;
  len = snprintf(buf, buf_len, "file blocks covered : %u\n", ei->ei_block);
  buf += len;
  len = snprintf(buf, buf_len, "block pointed to : %llu\n", ((__le64)ei->ei_leaf_hi << 32) | (__le64)ei->ei_leaf_lo);
  buf += len;

  len = snprintf(buf, buf_len, "\n");
  buf += len;
}

void ext4_show_stat_extent(struct ext4_extent *ext, char *buf, int32_t buf_len)
{
  int32_t len = 0;

  len = snprintf(buf, buf_len, "Extent leaf node :\n");
  buf += len;
  len = snprintf(buf, buf_len, "first file block : %u\n", ext->ee_block);
  buf += len;
  len = snprintf(buf, buf_len, "blocks : %u\n", ext->ee_len);
  buf += len;
  len = snprintf(buf, buf_len, "block pointed to : %llu\n", ((__le64)ext->ee_start_hi << 32) | (__le64)ext->ee_start_lo);
  buf += len;

  len = snprintf(buf, buf_len, "\n");
  buf += len;
}

void ext4_show_stat_dentry(struct ext4_dir_entry_2 *dentry, char *buf, int32_t buf_len)
{
  int32_t i = 0;
  const char *str = NULL;
  int32_t len = 0;

  len = snprintf(buf, buf_len, "Linear dentry :\n");
  buf += len;
  len = snprintf(buf, buf_len, "inode : %u\n", dentry->inode);
  buf += len;
  len = snprintf(buf, buf_len, "entry length : %u\n", dentry->rec_len);
  buf += len;
  len = snprintf(buf, buf_len, "name length : %u\n", dentry->name_len);
  buf += len;

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
  len = snprintf(buf, buf_len, "type : %s\n", str);
  buf += len;

  len = snprintf(buf, buf_len, "name : ");
  buf += len;
  for (i = 0; i < dentry->name_len; ++i) {
    len = snprintf(buf, buf_len, "%c", dentry->name[i]);
    buf += len;
  }

  len = snprintf(buf, buf_len, "\n\n");
  buf += len;
}
