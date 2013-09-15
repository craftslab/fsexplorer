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

#include "config.h"

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

#ifdef DEBUG
// Add code here
#endif

/*
 * Macro Definition
 */

/*
 * Type Definition
 */
struct fs_opt_t {
  int32_t (*mount) (const char *dev_name, const char *dir_name, const char *type, uint32_t flags, void *data);
  int32_t (*umount) (const char *name, int32_t flags);
};

/*
 * Function Declaration
 */
typedef int32_t (*fs_opt_init_t) (struct fs_opt_t *fs_opt);

#endif /* _LIBFS_H */
