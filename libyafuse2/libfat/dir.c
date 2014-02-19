/**
 * dir.c - directory of FAT.
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

#ifdef DEBUG
#define DEBUG_LIBFAT_DIR
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
static int32_t fat_fill_root_dent_sec(const struct fat_super_block *sb, int32_t *sector);

/*
 * Function Definition
 */
static int32_t fat_fill_root_dent_sec(const struct fat_super_block *sb, int32_t *sector)
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
  }

  return 0;
}

int32_t fat_fill_dent_start(const struct fat_super_block *sb, const struct msdos_dir_entry *dentry, int32_t *cluster, size_t *size)
{
  int32_t is_fat32_fs = 0;
  int32_t ret = 0;

  ret = fat_is_fat32_fs(sb, &is_fat32_fs);
  if (ret != 0) {
    return -1;
  }

  if (is_fat32_fs) {
    *cluster = ((int32_t)dentry->starthi << 16) | (int32_t)dentry->start;
  } else {
    *cluster = (int32_t)dentry->start;
  }

  *size = (size_t)dentry->size;

  return 0;
}

int32_t fat_fill_root_dentries(const struct fat_super_block *sb, int32_t *dentries)
{
  int32_t root_den_sec = 0;
  int64_t offset = 0;
  struct msdos_dir_entry dentry;
  int64_t sz = 0;
  int32_t i = 0;
  int32_t ret = 0;

  ret = fat_fill_root_dent_sec(sb, &root_den_sec);
  if (ret != 0) {
    return -1;
  }

  offset = root_den_sec * (int32_t)GET_UNALIGNED_LE16(sb->bs.sector_size);

  ret = io_seek(offset);
  if (ret != 0) {
    return -1;
  }

  sz = sizeof(struct msdos_dir_entry);

  ret = io_read((uint8_t *)&dentry, sz);
  if (ret != 0) {
    return -1;
  }

  /*
   * Long File Names (LFN), i.e., dslot, is used
   */
  if (dentry.attr == ATTR_EXT) {
    ret = io_read((uint8_t *)&dentry, sz);
    if (ret != 0) {
      return -1;
    }
  }

  while (dentry.name[0] != '\0') {
    ++i;

    ret = io_read((uint8_t *)&dentry, sz);
    if (ret != 0) {
      return -1;
    }

    /*
     * Long File Names (LFN), i.e., dslot, is used
     */
    if (dentry.name[0] != '\0' && dentry.attr == ATTR_EXT) {
      ret = io_read((uint8_t *)&dentry, sz);
      if (ret != 0) {
        return -1;
      }
    }
  }

  *dentries = i;

  return 0;
}

int32_t fat_fill_root_dentry(const struct fat_super_block *sb, int32_t dentries, struct msdos_dir_slot *dslot, struct msdos_dir_entry *dentry)
{
  int32_t root_den_sec = 0;
  int64_t offset = 0;
  int64_t sz = 0;
  int32_t i = 0;
  int32_t ret = 0;

  ret = fat_fill_root_dent_sec(sb, &root_den_sec);
  if (ret != 0) {
    return -1;
  }

  offset = root_den_sec * (int32_t)GET_UNALIGNED_LE16(sb->bs.sector_size);

  ret = io_seek(offset);
  if (ret != 0) {
    return -1;
  }

  sz = sizeof(struct msdos_dir_entry);

  for (i = 0; i < dentries; ++i) {
    ret = io_read((uint8_t *)&dslot[i], sz);
    if (ret != 0) {
      break;
    }

    /*
     * Long File Names (LFN), i.e., dslot, is used
     */
    if (dslot[i].attr == ATTR_EXT) {
      ret = io_read((uint8_t *)&dentry[i], sz);
      if (ret != 0) {
        break;
      }
    } else {
      memcpy((void *)&dentry[i], (const void *)&dslot[i], (size_t)sz);
    }
  }

  return ret;
}

int32_t fat_fill_dentries(const struct fat_super_block *sb, int32_t cluster, int32_t *dentries)
{
  int32_t sector = 0;
  int64_t offset = 0;
  struct msdos_dir_entry dentry;
  int64_t sz = 0;
  int32_t i = 1;
  int32_t ret = 0;

  ret = fat_fill_clus2sec(sb, cluster, &sector);
  if (ret != 0) {
    return -1;
  }

  offset = sector * (int32_t)GET_UNALIGNED_LE16(sb->bs.sector_size);

  ret = io_seek(offset);
  if (ret != 0) {
    return -1;
  }

  sz = sizeof(struct msdos_dir_entry);

  /*
   * Read FAT dentry of '.'
   */
  ret = io_read((uint8_t *)&dentry, sz);
  if (ret != 0) {
    return -1;
  }

  /*
   * Read FAT dentry of '..'
   */
  ret = io_read((uint8_t *)&dentry, sz);
  if (ret != 0) {
    return -1;
  }

  while (dentry.name[0] != '\0') {
    ++i;

    ret = io_read((uint8_t *)&dentry, sz);
    if (ret != 0) {
      return -1;
    }

    /*
     * Long File Names (LFN), i.e., dslot, is used
     */
    if (dentry.attr == ATTR_EXT) {
      ret = io_read((uint8_t *)&dentry, sz);
      if (ret != 0) {
        return -1;
      }
    }
  }

  *dentries = i;

  return 0;
}

int32_t fat_fill_dentry(const struct fat_super_block *sb, int32_t cluster, int32_t dentries, struct msdos_dir_slot *dslot, struct msdos_dir_entry *dentry)
{
  int32_t sector = 0;
  int64_t offset = 0;
  int64_t sz = 0;
  int32_t i = 0;
  int32_t ret = 0;

  ret = fat_fill_clus2sec(sb, cluster, &sector);
  if (ret != 0) {
    return -1;
  }

  offset = sector * (int32_t)GET_UNALIGNED_LE16(sb->bs.sector_size);

  ret = io_seek(offset);
  if (ret != 0) {
    return -1;
  }

  sz = sizeof(struct msdos_dir_entry);

  /*
   * Fill in dentry of '.'
   */
  memset((void *)&dslot[0], 0, (size_t)sz);

  ret = io_read((uint8_t *)&dentry[0], sz);
  if (ret != 0) {
    return -1;
  }

  /*
   * Fill in dentry of '..'
   */
  memset((void *)&dslot[1], 0, (size_t)sz);

  ret = io_read((uint8_t *)&dentry[1], sz);
  if (ret != 0) {
    return -1;
  }

  /*
   * Fill in other dentry
   */
  for (i = 2; i < dentries; ++i) {
    ret = io_read((uint8_t *)&dslot[i], sz);
    if (ret != 0) {
      break;
    }

    /*
     * Long File Names (LFN), i.e., dslot, is used
     */
    if (dslot[i].attr == ATTR_EXT) {
      ret = io_read((uint8_t *)&dentry[i], sz);
      if (ret != 0) {
        break;
      }
    } else {
      memcpy((void *)&dentry[i], (const void *)&dslot[i], (size_t)sz);
    }
  }

  return ret;
}

int32_t fat_dent_attr_is_dir(const struct msdos_dir_entry *dentry, int32_t *status)
{
  if (dentry->attr & ATTR_DIR) {
    *status = 1;
  } else {
    *status = 0;
  }

  return 0;
}
