/**
 * subsystem.h - The header of subsystem.
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

#ifndef _SUBSYSTEM_H
#define _SUBSYSTEM_H

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
/* Maximum length of subsystem name */
#define SS_NAME_LEN_MAX  (20)

/*
 * Type Definition
 */

/*
 * Function Declaration
 */
int32_t ss_create(const char *ss_name, const char *fs_name, int32_t *ret_val);
void ss_delete(int32_t ss_idx);

#endif /* _SUBSYSTEM_H */
