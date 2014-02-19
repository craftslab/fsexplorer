/**
 * types.h - The header of type declaration.
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

#ifndef _TYPES_H
#define _TYPES_H

#include "config.h"
#include <stdint.h>

#ifdef DEBUG
#define DEBUG_INCLUDE_BASE_TYPES
#endif

/*
 * Macro Definition
 */
#define __le64 u64
#define __le32 u32
#define __le16 u16

#define __be64 u64
#define __be32 u32
#define __be16 u16

#define __u64 u64
#define __u32 u32
#define __u16 u16
#define __u8 u8

#ifndef size_t
#define size_t uint32_t
#endif

#ifndef ssize_t
#define ssize_t int32_t
#endif

#define cpu_to_le32(x) ((uint32_t)(x))

#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

#define DIV_ROUND_UP(x, y) (((x) + (y) - 1)/(y))
#define ALIGN(x, y) ((y) * DIV_ROUND_UP((x), (y)))

#define IS_POWER_OF_2(n) (n != 0 && ((n & (n - 1)) == 0))

#define GET_UNALIGNED_LE16(p) (((uint16_t)p[1] << 8) | ((uint16_t)p[0]))

#define offsetof(type, member) ((char *) &((type *)0)->member)

#ifdef CMAKE_COMPILER_IS_GNUCC
#define container_of(ptr, type, member) ({ \
      (type *)((char *)ptr - offsetof(type, member));})
#else
#define container_of(ptr, type, member) ( \
      (type *)((char *)ptr - offsetof(type, member)))
#endif

#define list_entry(ptr, type, member) \
  container_of(ptr, type, member)

#ifdef CMAKE_COMPILER_IS_GNUCC
#define list_for_each_entry(pos, head, member) \
  for (pos = list_entry((head)->next, typeof(*pos), member); \
       &pos->member != (head); \
       pos = list_entry(pos->member.next, typeof(*pos), member))

#define list_for_each_entry_reverse(pos, head, member) \
  for (pos = list_entry((head)->prev, typeof(*pos), member); \
       &pos->member != (head); \
       pos = list_entry(pos->member.prev, typeof(*pos), member))
#endif

/*
 * Type Definition
 */
#ifndef CMAKE_COMPILER_IS_GNUCC
#define inline __inline
#endif /* CMAKE_COMPILER_IS_GNUCC */

typedef uint8_t bool;
typedef unsigned long long u64;
typedef signed long long s64;
typedef unsigned int u32;
typedef unsigned short int u16;
typedef unsigned char u8;
#ifndef CMAKE_COMPILER_IS_GNUCC
typedef unsigned int dev_t;
#endif /* CMAKE_COMPILER_IS_GNUCC */

struct list_head {
  struct list_head *next, *prev;
};

/*
 * Function Declaration
 */
static inline void list_init(struct list_head *list)
{
  list->next = list;
  list->prev = list;
}

static inline void __list_add(struct list_head *new,
                              struct list_head *prev,
                              struct list_head *next)
{
  next->prev = new;
  new->next = next;
  new->prev = prev;
  prev->next = new;
}

static inline void list_add(struct list_head *new, struct list_head *head)
{
  __list_add(new, head, head->next);
}

static inline void __list_del(struct list_head *prev, struct list_head *next)
{
  next->prev = prev;
  prev->next = next;
}

static inline void __list_del_entry(struct list_head *entry)
{
  __list_del(entry->prev, entry->next);
}

static inline void list_del_init(struct list_head *entry)
{
  __list_del_entry(entry);
  list_init(entry);
}

static inline int list_empty(const struct list_head *head)
{
  return head->next == head;
}

#endif /* _TYPES_H */
