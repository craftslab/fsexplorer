/**
 * debug.h - The header of debugging for filesystem.
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

#ifndef _DEBUG_H
#define _DEBUG_H

#include "config.h"
#include <stdint.h>

#ifdef DEBUG
// Add code here
#endif

/*
 * Macro Definition
 */
#ifdef CMAKE_COMPILER_IS_GNUCC
#ifdef DEBUG
#define info(fmt, args...) \
  do { \
    fprintf(stdout, "info: %s: " fmt "\n", __func__, ## args);  \
  } while (0)

#define warn(fmt, args...) \
  do { \
    fprintf(stderr, "warning: %s: " fmt "\n", __func__, ## args); \
  } while (0)

#define error(fmt, args...) \
  do { \
    fprintf(stderr, "error: %s: " fmt "\n", __func__, ## args); \
  } while (0)
#else
#define info(fmt, args...)                      \
  do { \
    fprintf(stdout, "info: " fmt "\n", ## args);                  \
  } while (0)

#define warn(fmt, args...) \
  do { \
    fprintf(stderr, "warning: " fmt "\n", ## args); \
  } while (0)

#define error(fmt, args...) \
  do { \
    fprintf(stderr, "error: " fmt "\n", ## args); \
  } while (0)
#endif /* DEBUG */
#else
#ifdef DEBUG
#define info(fmt, ...) \
  do { \
    fprintf(stdout, "info: %s: " fmt "\n", __func__, __VA_ARGS__);  \
  } while (0)

#define warn(fmt, ...) \
  do { \
    fprintf(stderr, "warning: %s: " fmt "\n", __func__, __VA_ARGS__); \
  } while (0)

#define error(fmt, ...) \
  do { \
    fprintf(stderr, "error: %s: " fmt "\n", __func__, __VA_ARGS__); \
  } while (0)
#else
#define info(fmt, ...)                      \
  do { \
    fprintf(stdout, "info: " fmt "\n", __VA_ARGS__);                  \
  } while (0)

#define warn(fmt, ...) \
  do { \
    fprintf(stderr, "warning: " fmt "\n", __VA_ARGS__); \
  } while (0)

#define error(fmt, ...) \
  do { \
    fprintf(stderr, "error: " fmt "\n", __VA_ARGS__); \
  } while (0)
#endif /* DEBUG */
#endif /* CMAKE_COMPILER_IS_GNUCC */

/*
 * Type Definition
 */

/*
 * Function Declaration
 */

#endif /* _DEBUG_H */
