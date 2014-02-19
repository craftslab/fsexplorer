/**
 * jbd2.h - The header of jbd2.
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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _LINUX_JBD2_H
#define _LINUX_JBD2_H

#define JBD2_DEBUG
#define jfs_debug jbd_debug

#define journal_oom_retry 1

#undef JBD2_PARANOID_IOFAIL

#define JBD2_DEFAULT_MAX_COMMIT_AGE 5

#ifdef CMAKE_COMPILER_IS_GNUCC
#define jbd_debug(f, a...)
#else
#define jbd_debug(f, ...)
#endif /* CMAKE_COMPILER_IS_GNUCC */

#define JBD2_MIN_JOURNAL_BLOCKS 1024

#define JBD2_MAGIC_NUMBER 0xc03b3998U  

#define JBD2_DESCRIPTOR_BLOCK 1
#define JBD2_COMMIT_BLOCK 2
#define JBD2_SUPERBLOCK_V1 3
#define JBD2_SUPERBLOCK_V2 4
#define JBD2_REVOKE_BLOCK 5

typedef struct journal_header_s
{
 __be32 h_magic;
 __be32 h_blocktype;
 __be32 h_sequence;
} journal_header_t;

#define JBD2_CRC32_CHKSUM 1
#define JBD2_MD5_CHKSUM 2
#define JBD2_SHA1_CHKSUM 3

#define JBD2_CRC32_CHKSUM_SIZE 4

#define JBD2_CHECKSUM_BYTES (32 / sizeof(u32))

struct commit_header {
 __be32 h_magic;
 __be32 h_blocktype;
 __be32 h_sequence;
 unsigned char h_chksum_type;
 unsigned char h_chksum_size;
 unsigned char h_padding[2];
 __be32 h_chksum[JBD2_CHECKSUM_BYTES];
 __be64 h_commit_sec;
 __be32 h_commit_nsec;
};

typedef struct journal_block_tag_s
{
 __be32 t_blocknr;
 __be32 t_flags;
 __be32 t_blocknr_high;
} journal_block_tag_t;

#define JBD2_TAG_SIZE32 (offsetof(journal_block_tag_t, t_blocknr_high))
#define JBD2_TAG_SIZE64 (sizeof(journal_block_tag_t))

typedef struct jbd2_journal_revoke_header_s
{
 journal_header_t r_header;
 __be32 r_count;
} jbd2_journal_revoke_header_t;

#define JBD2_FLAG_ESCAPE 1  
#define JBD2_FLAG_SAME_UUID 2  
#define JBD2_FLAG_DELETED 4  
#define JBD2_FLAG_LAST_TAG 8  

typedef struct journal_superblock_s
{

 journal_header_t s_header;

 __be32 s_blocksize;
 __be32 s_maxlen;
 __be32 s_first;

 __be32 s_sequence;
 __be32 s_start;

 __be32 s_errno;

 __be32 s_feature_compat;
 __be32 s_feature_incompat;
 __be32 s_feature_ro_compat;

 __u8 s_uuid[16];

 __be32 s_nr_users;

 __be32 s_dynsuper;

 __be32 s_max_transaction;
 __be32 s_max_trans_data;

 __u32 s_padding[44];

 __u8 s_users[16*48];

} journal_superblock_t;

#define JBD2_HAS_COMPAT_FEATURE(j,mask)   ((j)->j_format_version >= 2 &&   ((j)->j_superblock->s_feature_compat & cpu_to_be32((mask))))
#define JBD2_HAS_RO_COMPAT_FEATURE(j,mask)   ((j)->j_format_version >= 2 &&   ((j)->j_superblock->s_feature_ro_compat & cpu_to_be32((mask))))
#define JBD2_HAS_INCOMPAT_FEATURE(j,mask)   ((j)->j_format_version >= 2 &&   ((j)->j_superblock->s_feature_incompat & cpu_to_be32((mask))))

#define JBD2_FEATURE_COMPAT_CHECKSUM 0x00000001

#define JBD2_FEATURE_INCOMPAT_REVOKE 0x00000001
#define JBD2_FEATURE_INCOMPAT_64BIT 0x00000002
#define JBD2_FEATURE_INCOMPAT_ASYNC_COMMIT 0x00000004

#define JBD2_KNOWN_COMPAT_FEATURES JBD2_FEATURE_COMPAT_CHECKSUM
#define JBD2_KNOWN_ROCOMPAT_FEATURES 0
#define JBD2_KNOWN_INCOMPAT_FEATURES (JBD2_FEATURE_INCOMPAT_REVOKE |   JBD2_FEATURE_INCOMPAT_64BIT |   JBD2_FEATURE_INCOMPAT_ASYNC_COMMIT)

#define BJ_None 0  
#define BJ_Metadata 1  
#define BJ_Forget 2  
#define BJ_IO 3  
#define BJ_Shadow 4  
#define BJ_LogCtl 5  
#define BJ_Reserved 6  
#define BJ_Types 7

#endif

