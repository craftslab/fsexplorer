/**
 * platform_win32.cpp - The entry of platform_win32
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

#include <Windows.h>

#include "platform.h"

/*
 * Macro Definition
 */
#define LIB_NAME "fs.dll"
#define LIB_SYMBOL "fs_opt_init"

/*
 * Type Definition
 */

/*
 * Global Variable Definition
 */
static void *libHandle;

/*
 * Function Declaration
 */

/*
 * Function Definition
 */
bool initOpt(fs_opt_t *opt)
{
  fs_opt_init_t optHandle;

  libHandle = LoadLibrary((LPCWSTR)LIB_NAME);
  if (!libHandle) {
    goto initOpt_exit;
  }

  *(void **)(&optHandle) = GetProcAddress((HMODULE)libHandle, (LPCSTR)LIB_SYMBOL);
  if (!optHandle) {
    goto initOpt_exit;
  }

  if (optHandle(opt) != 0) {
    goto initOpt_exit;
  }

  return true;

 initOpt_exit:

  if (libHandle) {
    (void)FreeLibrary((HMODULE)libHandle);
  }

  return false;
}

void deinitOpt()
{
  if (libHandle) {
    (void)FreeLibrary((HMODULE)libHandle);
  }
}
