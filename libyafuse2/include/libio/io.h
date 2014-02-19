/**
 * io.h - The header of IO interface.
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

#ifndef _IO_H
#define _IO_H

#include "config.h"
#include <stdint.h>
#include <sys/types.h>

#ifdef DEBUG
#define DEBUG_INCLUDE_LIBIO_IO
#endif

#include "include/base/types.h"

/*
 * Macro Definition
 */

/*
 * Type Definition
 */

/*
 * Function Declaration
 */
int32_t io_open(const char *fs_name);
void io_close(void);
int32_t io_seek(int64_t offset);
int32_t io_read(uint8_t *data, int64_t len);
int32_t io_write(uint8_t *data, int64_t len);

#endif /* _IO_H */
