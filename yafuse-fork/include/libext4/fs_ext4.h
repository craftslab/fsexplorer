/**
 * fs_ext4.h - The header of Ext4 filesystem.
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

#ifndef _FS_EXT4_H
#define _FS_EXT4_H

#include "config.h"

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

#ifdef DEBUG
// Add code here
#endif

#include "include/fs.h"

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
fs_opt_tbl_t* fs_opt_tbl_init(void);

#endif /* _FS_EXT4_H */
