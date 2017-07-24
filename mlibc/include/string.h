/* -*-comment-start: "//";comment-end:""-*-
 * Mes --- Maxwell Equations of Software
 * Copyright © 2017 Jan Nieuwenhuizen <janneke@gnu.org>
 *
 * This file is part of Mes.
 *
 * Mes is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * Mes is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Mes.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __MES_STRING_H
#define __MES_STRING_H 1

#if __GNUC__ && POSIX
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#undef __MES_STRING_H
#include_next <string.h>

#else // ! (__GNUC__ && POSIX)

#ifndef __MES_SIZE_T
#define __MES_SIZE_T
typedef unsigned long size_t;
#endif

#ifndef __MES_SSIZE_T
#define __MES_SSIZE_T
typedef long ssize_t;
#endif

void *memcpy (void *dest, void const *src, size_t n);
void *memmove (void *dest, void const *src, size_t n);
void *memset (void *s, int c, size_t n);
int memcmp (void const *s1, void const *s2, size_t n);

size_t strlen (char const*);
char *strchr (char const *s, int c);
int strcmp (char const*, char const*);
char *strcpy (char *dest, char const *src);
int strncmp (char const*, char const*, size_t);
char *strrchr (char const *s, int c);
char *strstr (char const *haystack, char const *needle);

#endif // ! (__GNUC__ && POSIX)

#endif // __MES_STRING_H
