/**
 * super.c - Superblock of FAT.
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

#include "config.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>

#ifdef DEBUG
#define DEBUG_LIBFAT_SUPER
#endif

#include "include/base/debug.h"
#include "include/base/types.h"
#include "include/libio/io.h"
#include "include/libfat/libfat.h"

/*
 * Macro Definition
 */

/*
 * Type Definition
 */

/*
 * Global Variable Definition
 */

/*
 * Function Declaration
 */
static int32_t fat_is_valid_sec_sz(uint8_t *sec_sz, uint32_t len);
static int32_t fat_is_valid_media(uint32_t media);

/*
 * Function Definition
 */
static int32_t fat_is_valid_sec_sz(uint8_t *sec_sz, uint32_t len)
{
  uint32_t val = 0;

  if (len != 2) {
    return 0;
  }

  val = (uint32_t)GET_UNALIGNED_LE16(sec_sz);

  return IS_POWER_OF_2(val) && (val >= SECTOR_SIZE) && (val <= SECTOR_SIZE_MAX);
}

static int32_t fat_is_valid_media(uint32_t media)
{
  return (media >= 0xF8) || (media == 0xF0);
}

int32_t fat_fill_sb(struct fat_super_block *sb)
{
  int64_t offset = 0;
  int64_t sz = 0;
  int32_t is_fat32_fs = 0;
  int32_t ret = 0;

  /*
   * Fill in FAT boot sector
   */
  offset = 0;
  ret = io_seek(offset);
  if (ret != 0) {
    return -1;
  }

  sz = sizeof(struct fat_boot_sector);
  ret = io_read((uint8_t *)&sb->bs, sz);
  if (ret != 0) {
    memset((void *)&sb->bs, 0, (size_t)sz);
    return -1;
  }

  if (!fat_is_valid_sec_sz((uint8_t *)sb->bs.sector_size, 2)
      || !IS_POWER_OF_2(sb->bs.sec_per_clus)
      || sb->bs.reserved == 0
      || sb->bs.fats == 0
      || !fat_is_valid_media((uint32_t)sb->bs.media)) {
    return -1;
  }

  ret = fat_is_fat32_fs((const struct fat_super_block *)sb, &is_fat32_fs);
  if (ret != 0) {
    return -1;
  }

  /*
   * Fill in FAT boot bsx
   */
  if (is_fat32_fs) {
    offset = FAT32_BSX_OFFSET;
  } else {
    offset = FAT16_BSX_OFFSET;
  }

  ret = io_seek(offset);
  if (ret != 0) {
    return -1;
  }

  sz = sizeof(struct fat_boot_bsx);
  ret = io_read((uint8_t *)&sb->bb, sz);
  if (ret != 0) {
    memset((void *)&sb->bb, 0, (size_t)sz);
    return -1;
  }

  if (is_fat32_fs) {
    /*
     * Fill in FAT32 boot fsinfo
     */
    offset = sb->bs.info_sector == 0 ? GET_UNALIGNED_LE16(sb->bs.sector_size) : sb->bs.info_sector * GET_UNALIGNED_LE16(sb->bs.sector_size);
    ret = io_seek(offset);
    if (ret != 0) {
      return -1;
    }

    sz = sizeof(struct fat_boot_fsinfo);
    ret = io_read((uint8_t *)&sb->bf, sz);
    if (ret != 0) {
      memset((void *)&sb->bf, 0,(size_t)sz);
      return -1;
    }

    if (sb->bf.signature1 != FAT_FSINFO_SIG1 || sb->bf.signature2 != FAT_FSINFO_SIG2) {
      return -1;
    }
  }

  return 0;
}

int32_t fat_is_fat32_fs(const struct fat_super_block *sb, int32_t *status)
{
  *status = (sb->bs.fat_length == 0) && (sb->bs.fat32_length != 0);

  return 0;
}

int32_t fat_fill_clus2sec(const struct fat_super_block *sb, int32_t cluster, int32_t *sector)
{
  int32_t is_fat32_fs = 0;
  int32_t ret = 0;

  ret = fat_is_fat32_fs(sb, &is_fat32_fs);
  if (ret != 0) {
    return -1;
  }

  if (is_fat32_fs) {
    *sector = (int32_t)sb->bs.reserved + ((int32_t)sb->bs.fats * (int32_t)sb->bs.fat32_length);
  } else {
    *sector = (int32_t)sb->bs.reserved + ((int32_t)sb->bs.fats * (int32_t)sb->bs.fat_length);
    *sector += ((int32_t)GET_UNALIGNED_LE16(sb->bs.dir_entries) * sizeof(struct msdos_dir_entry)) / ((int32_t)GET_UNALIGNED_LE16(sb->bs.sector_size));
  }

  *sector += (cluster - FAT_START_ENT) * sb->bs.sec_per_clus;

  return 0;
}
