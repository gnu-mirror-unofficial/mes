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

#include "30-test.i"


struct baz {
  int i;
};

struct foo {
  int **bar;
};

void
add (void *ptab)
{
  void ***x = (void***)ptab;
 bla:
  *(void***)ptab = 0x11223344;
}

void
add2 (void *ptab)
{
  void ***x = (void***)ptab;
 bla:
  *x = 0x22334455;
}

int
test ()
{
  int i;
  int *p = &i;
  struct foo f;
  f.bar = &p;
  eputs ("f.bar:"); eputs (itoa (f.bar)); eputs ("\n");
  add (&f.bar);
  eputs ("f.bar:"); eputs (itoa (f.bar)); eputs ("\n");
  if (f.bar != 0x11223344) return 1;
  add2 (&f.bar);
  // FIXME
  // eputs ("f.bar:"); eputs (itoa (f.bar)); eputs ("\n");
  // if (f.bar != 0x22334455) return 2;

  return 0;
}
