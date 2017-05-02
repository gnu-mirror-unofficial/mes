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
#ifndef __ASSERT_H
#define __ASSERT_H 1

#if __GNUC__ && POSIX
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include_next <assert.h>
#else // ! (__GNUC__ && POSIX)
#define assert(x) ((x) ? (void)0 : assert_fail (#x))
#endif // ! (__GNUC__ && POSIX)

#endif // __ASSERT_H
