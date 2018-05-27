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
#define SYS_read   "0x03"
#define SYS_write  "0x04"
#define SYS_open   "0x05"
#define SYS_close  "0x06"
#define SYS_unlink "0x0a"
#define SYS_lseek  "0x13"
#define SYS_access "0x21"
#define SYS_brk    "0x2d"
#define SYS_stat   "0x6a"
#define SYS_fstat  "0x6c"
#define SYS_fsync  "0x76"
#define SYS_getcwd "0xb7"

int
close (int fd)
{
#if !__TINYC__
  int r;
  asm (
       "mov    %0,%%ebx\n\t"
       "mov    $"SYS_close",%%eax\n\t"
       "int    $0x80"
       : "=r" (r)
       : "" (fd)
       );
  return r;
#endif
}

int
unlink (char const *file_name)
{
#if !__TINYC__
  int r;
  asm (
       "mov    %0,%%ebx\n\t"
       "mov    $"SYS_unlink",%%eax\n\t"
       "int    $0x80"
       : "=r" (r)
       : "" (file_name)
       );
  return r;
#endif
}

off_t
lseek (int fd, off_t offset, int whence)
{
#if !__TINYC__
  int r;
  asm (
       "mov    %1,%%ebx\n\t"
       "mov    %2,%%ecx\n\t"
       "mov    %3,%%edx\n\t"

       "mov    $"SYS_lseek",%%eax\n\t"
       "int  $0x80\n\t"

       "mov    %%eax,%0\n\t"
       : "=r" (r)
       : "" (fd), "" (offset), "" (whence)
       : "eax", "ebx", "ecx", "edx"
       );
  return r;
#endif
}


int
stat (char const *file_name, struct stat *statbuf)
{
#if !__TINYC__
  int r;
  asm (
       "mov    %1,%%ebx\n\t"
       "mov    %2,%%ecx\n\t"

       "mov    $"SYS_stat",%%eax\n\t"
       "int  $0x80\n\t"

       "mov    %%eax,%0\n\t"
       : "=r" (r)
       : "" (file_name), "" (statbuf)
       : "eax", "ebx", "ecx"
       );
  return r;
#endif
}

int
fstat (int fd, struct stat *statbuf)
{
#if !__TINYC__
  int r;
  asm (
       "mov    %1,%%ebx\n\t"
       "mov    %2,%%ecx\n\t"

       "mov    $"SYS_fstat",%%eax\n\t"
       //"mov    $"SYS_oldfstat",%%eax\n\t"
       "int  $0x80\n\t"

       "mov    %%eax,%0\n\t"
       : "=r" (r)
       : "" (fd), "" (statbuf)
       : "eax", "ebx", "ecx"
       );
  return r;
#endif
}

char *
getcwd (char *buf, size_t size)
{
#if !__TINYC__
  int r;
  asm (
       "mov    %1,%%ebx\n\t"
       "mov    %2,%%ecx\n\t"

       "mov    $"SYS_getcwd",%%eax\n\t"
       "int  $0x80\n\t"

       "mov    %%eax,%0\n\t"
       : "=r" (r)
       : "" (buf), "" (size)
       : "eax", "ebx", "ecx"
       );
  return r;
#endif
}
