/* -*-comment-start: "//";comment-end:""-*-
 * Mes --- Maxwell Equations of Software
 * Copyright © 2016,2017 Jan (janneke) Nieuwenhuizen <janneke@gnu.org>
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

#define SYS_exit   "0x01"
#define SYS_write  "0x04"

void
_exit (int code)
{
  asm (
       "mov    $"SYS_exit",%%eax\n\t"
       "mov    %0,%%ebx\n\t"
       "int    $0x80\n\t"
       : // no outputs "=" (r)
       : "rm" (code)
       );
  // not reached
  _exit (0);
}

ssize_t
_write (int filedes, void const *buffer, size_t size)
{
  int r;
  asm (
       "mov    $"SYS_write",%%eax\n\t"
       "mov    %1,%%ebx\n\t"
       "mov    %2,%%ecx\n\t"
       "mov    %3,%%edx\n\t"
       "int    $0x80\n\t"
       "mov    %%eax,%0\n\t"
       : "=r" (r)
       : "rm" (filedes), "rm" (buffer), "rm" (size)
       : "eax", "ebx", "ecx", "edx"
       );
  return r;
}