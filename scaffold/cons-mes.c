/* -*-comment-start: "//";comment-end:""-*-
 * Mes --- Maxwell Equations of Software
 * Copyright © 2016,2017 Jan Nieuwenhuizen <janneke@gnu.org>
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

#define MES_MINI 1
#define FIXED_PRIMITIVES 0

#if __GNUC__
#define FIXME_NYACC 1
#define  __NYACC__ 0
#define NYACC_CAR
#define NYACC_CDR
#else
#define  __NYACC__ 1
#define NYACC_CAR nyacc_car
#define NYACC_CDR nyacc_cdr
#endif

char arena[2000];
//char buf0[400];

int g_stdin = 0;

#if __GNUC__
typedef long size_t;
void *malloc (size_t i);
int open (char const *s, int mode);
int read (int fd, void* buf, size_t n);
void write (int fd, char const* s, int n);

void
exit (int code)
{
  asm (
       "movl %0,%%ebx\n\t"
       "movl $1,%%eax\n\t"
       "int  $0x80"
       : // no outputs "=" (r)
       : "" (code)
       );
  // not reached
  exit (0);
}

char const*
getenv (char const* p)
{
  return 0;
}

int
read (int fd, void* buf, size_t n)
{
  int r;
  //syscall (SYS_write, fd, s, n));
  asm (
       "movl %1,%%ebx\n\t"
       "movl %2,%%ecx\n\t"
       "movl %3,%%edx\n\t"
       "movl $0x3,%%eax\n\t"
       "int  $0x80\n\t"
       "mov %%eax,%0\n\t"
       : "=r" (r)
       : "" (fd), "" (buf), "" (n)
       : "eax", "ebx", "ecx", "edx"
       );
  return r;
}

int
open (char const *s, int mode)
{
  int r;
  //syscall (SYS_open, mode));
  asm (
       "mov %1,%%ebx\n\t"
       "mov %2,%%ecx\n\t"
       "mov $0x5,%%eax\n\t"
       "int $0x80\n\t"
       "mov %%eax,%0\n\t"
       : "=r" (r)
       : "" (s), "" (mode)
       : "eax", "ebx", "ecx"
       );
  return r;
}

int
getchar ()
{
  char c;
  int r = read (g_stdin, &c, 1);
  if (r < 1) return -1;
  return c;
}

void
write (int fd, char const* s, int n)
{
  int r;
  //syscall (SYS_write, fd, s, n));
  asm (
       "mov %0,%%ebx\n\t"
       "mov %1,%%ecx\n\t"
       "mov %2,%%edx\n\t"

       "mov $0x4, %%eax\n\t"
       "int $0x80\n\t"
       : // no outputs "=" (r)
       : "" (fd), "" (s), "" (n)
       : "eax", "ebx", "ecx", "edx"
       );
}

int
putchar (int c)
{
  //write (STDOUT, s, strlen (s));
  //int i = write (STDOUT, s, strlen (s));
  write (1, (char*)&c, 1);
  return 0;
}

void *
malloc (size_t size)
{
  int *n;
  int len = size + sizeof (size);
  //n = mmap (0, len, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0 );
  *n = len;
  return (void*)(n+1);
}

void
free (void *p)
{
  int *n = (int*)p-1;
  //munmap ((void*)p, *n);
}

#define EOF -1
#define STDIN 0
#define STDOUT 1
#define STDERR 2

size_t
strlen (char const* s)
{
  int i = 0;
  while (s[i]) i++;
  return i;
}

int
strcmp (char const* a, char const* b)
{
  while (*a && *b && *a == *b) {a++;b++;}
  return *a - *b;
}

int
puts (char const* s)
{
  //write (STDOUT, s, strlen (s));
  //int i = write (STDOUT, s, strlen (s));
  int i = strlen (s);
  write (1, s, i);
  return 0;
}

int
eputs (char const* s)
{
  //write (STDERR, s, strlen (s));
  //int i = write (STDERR, s, strlen (s));
  int i = strlen (s);
  write (2, s, i);
  return 0;
}

char const*
itoa (int x)
{
  static char buf[10];
  char *p = buf+9;
  *p-- = 0;

  int sign = x < 0;
  if (sign)
    x = -x;
  
  do
    {
      *p-- = '0' + (x % 10);
      x = x / 10;
    } while (x);

  if (sign)
    *p-- = '-';

  return p+1;
}
#endif

void
assert_fail (char* s)
{
  eputs ("assert fail:");
#if __GNUC__
  eputs (s);
#endif
  eputs ("\n");
#if __GNUC__
  *((int*)0) = 0;
#endif
}

#if __GNUC__
#define assert(x) ((x) ? (void)0 : assert_fail ("boo:" #x))
#else
//#define assert(x) ((x) ? (void)0 : assert_fail ("boo:" #x))
#define assert(x) ((x) ? (void)0 : assert_fail (0))
#endif

typedef int SCM;

#if __GNUC__
int g_debug = 0;
#endif

int g_free = 0;

SCM g_symbols = 0;
SCM g_stack = 0;
// a/env
SCM r0 = 0;
// param 1
SCM r1 = 0;
// save 2+load/dump
SCM r2 = 0;
// continuation
SCM r3 = 0;

#if __NYACC__ || FIXME_NYACC
enum type_t {CHAR, CLOSURE, CONTINUATION, TFUNCTION, KEYWORD, MACRO, NUMBER, PAIR, REF, SPECIAL, TSTRING, SYMBOL, VALUES, TVECTOR, BROKEN_HEART};
#else
enum type_t {CHAR, CLOSURE, CONTINUATION, FUNCTION, KEYWORD, MACRO, NUMBER, PAIR, REF, SPECIAL, STRING, SYMBOL, VALUES, VECTOR, BROKEN_HEART};
#endif

struct scm {
  enum type_t type;
  SCM car;
  SCM cdr;
};

typedef int (*f_t) (void);
struct function {
  int (*function) (void);
  int arity;
};

struct scm *g_cells = arena;

//scm *g_news = 0;

// struct scm scm_nil = {SPECIAL, "()"};
// struct scm scm_f = {SPECIAL, "#f"};
// struct scm scm_t = {SPECIAL, "#t"};
// struct scm_dot = {SPECIAL, "."};
// struct scm_arrow = {SPECIAL, "=>"};
// struct scm_undefined = {SPECIAL, "*undefined*"};
// struct scm_unspecified = {SPECIAL, "*unspecified*"};
// struct scm_closure = {SPECIAL, "*closure*"};
// struct scm_circular = {SPECIAL, "*circular*"};
// struct scm_begin = {SPECIAL, "*begin*"};

// struct scm_vm_apply = {SPECIAL, "core:apply"};
// struct scm_vm_apply2 = {SPECIAL, "*vm-apply2*"};

// struct scm_vm_eval = {SPECIAL, "core:eval"};

// struct scm_vm_begin = {SPECIAL, "*vm-begin*"};
// //scm scm_vm_begin_read_input_file = {SPECIAL, "*vm-begin-read-input-file*"};
// struct scm_vm_begin2 = {SPECIAL, "*vm-begin2*"};

// struct scm_vm_return = {SPECIAL, "*vm-return*"};

// //#include "mes.symbols.h"

#define cell_nil 1
#define cell_f 2
#define cell_t 3
#define cell_dot 4
// #define cell_arrow 5
#define cell_undefined 6
#define cell_unspecified 7
#define cell_closure 8
#define cell_circular 9
#define cell_begin 10
#define cell_symbol_dot 11
#define cell_symbol_lambda 12
#define cell_symbol_begin 13
#define cell_symbol_if 14
#define cell_symbol_quote 15
#define cell_symbol_set_x 16

#define cell_vm_apply 45
#define cell_vm_apply2 46

#define cell_vm_eval 47

#define cell_vm_begin 56
//#define cell_vm_begin_read_input_file 57
#define cell_vm_begin2 58

#define cell_vm_return 63

SCM tmp;
SCM tmp_num;
SCM tmp_num2;

int ARENA_SIZE = 200;
struct function functions[2];
int g_function = 0;


#if __GNUC__
//FIXME
SCM make_cell (SCM type, SCM car, SCM cdr);
#endif
struct function fun_make_cell = {&make_cell, 3};
struct scm scm_make_cell = {TFUNCTION,0,0};
   //, "make-cell", 0};
SCM cell_make_cell;

#if __GNUC__
//FIXME
SCM cons (SCM x, SCM y);
#endif
struct function fun_cons = {&cons, 2};
struct scm scm_cons = {TFUNCTION,0,0};
  // "cons", 0};
SCM cell_cons;

#if __GNUC__
//FIXME
SCM car (SCM x);
#endif
struct function fun_car = {&car, 1};
struct scm scm_car = {TFUNCTION,0,0};
  // "car", 0};
SCM cell_car;

#if __GNUC__
//FIXME
SCM cdr (SCM x);
#endif
struct function fun_cdr = {&cdr, 1};
struct scm scm_cdr = {TFUNCTION,0,0};
// "cdr", 0};
SCM cell_cdr;

// SCM eq_p (SCM x, SCM y);
// struct function fun_eq_p = {&eq_p, 2};
// scm scm_eq_p = {TFUNCTION,0,0};// "eq?", 0};
// SCM cell_eq_p;

#define TYPE(x) (g_cells[x].type)

#define CAR(x) g_cells[x].car
#define LENGTH(x) g_cells[x].car
#define STRING(x) g_cells[x].car

#define CDR(x) g_cells[x].cdr
#if __GNUC__
//#define CLOSURE(x) g_cells[x].closure
#endif
#define CONTINUATION(x) g_cells[x].cdr
#if __GNUC__
//#define FUNCTION(x) functions[g_cells[x].function]
#endif

#define FUNCTION(x) functions[g_cells[x].cdr]
#define VALUE(x) g_cells[x].cdr
#define VECTOR(x) g_cells[x].cdr

#define MAKE_CHAR(n) make_cell (tmp_num_ (CHAR), 0, tmp_num2_ (n))
//#define MAKE_CONTINUATION(n) make_cell (tmp_num_ (CONTINUATION), n, g_stack)
#define MAKE_NUMBER(n) make_cell (tmp_num_ (NUMBER), 0, tmp_num2_ (n))
//#define MAKE_REF(n) make_cell (tmp_num_ (REF), n, 0)


#define CAAR(x) CAR (CAR (x))
// #define CDAR(x) CDR (CAR (x))
#define CADAR(x) CAR (CDR (CAR (x)))
// #define CADDR(x) CAR (CDR (CDR (x)))
// #define CDDDR(x) CDR (CDR (CDR (x)))
#define CDADAR(x) CAR (CDR (CAR (CDR (x))))
#define CADR(x) CAR (CDR (x))


#if __NYACC__ || FIXME_NYACC
#define MAKE_STRING(x) make_cell (tmp_num_ (TSTRING), x, 0)
// #else
// #define MAKE_STRING(x) make_cell (tmp_num_ (STRING), x, 0)
#endif

SCM
alloc (int n)
{
#if __GNUC__
  //FIXME GNUC
  assert (g_free + n < ARENA_SIZE);
#endif
  SCM x = g_free;
  g_free += n;
  return x;
}

SCM
make_cell (SCM type, SCM car, SCM cdr)
{
  SCM x = alloc (1);
#if __GNUC__
  //FIXME GNUC
  assert (TYPE (type) == NUMBER);
#endif
  TYPE (x) = VALUE (type);
  if (VALUE (type) == CHAR || VALUE (type) == NUMBER) {
    if (car) CAR (x) = CAR (car);
    if (cdr) CDR(x) = CDR(cdr);
  }
  else if (VALUE (type) == TFUNCTION) {
    if (car) CAR (x) = car;
    if (cdr) CDR(x) = CDR(cdr);
  }
  else {
    CAR (x) = car;
    CDR(x) = cdr;
  }
  return x;
}

SCM
tmp_num_ (int x)
{
  VALUE (tmp_num) = x;
  return tmp_num;
}

SCM
tmp_num2_ (int x)
{
  VALUE (tmp_num2) = x;
  return tmp_num2;
}

SCM
cons (SCM x, SCM y)
{
  puts ("cons x=");
#if __GNUC__
  puts (itoa (x));
#endif
  puts ("\n");
  VALUE (tmp_num) = PAIR;
  return make_cell (tmp_num, x, y);
}

SCM
car (SCM x)
{
  puts ("car x=");
#if __GNUC__
  puts (itoa (x));
#endif
  puts ("\n");
#if MES_MINI
  //Nyacc
  //assert ("!car");
#else
  if (TYPE (x) != PAIR) error (cell_symbol_not_a_pair, cons (x, cell_symbol_car));
#endif
  return CAR (x);
}

SCM
cdr (SCM x)
{
  puts ("cdr x=");
#if __GNUC__
  puts (itoa (x));
#endif
  puts ("\n");
#if MES_MINI
  //Nyacc
  //assert ("!cdr");
#else
  if (TYPE (x) != PAIR) error (cell_symbol_not_a_pair, cons (x, cell_symbol_cdr));
#endif
  return CDR(x);
}

// SCM
// eq_p (SCM x, SCM y)
// {
//   return (x == y
//           || ((TYPE (x) == KEYWORD && TYPE (y) == KEYWORD
//                && STRING (x) == STRING (y)))
//           || (TYPE (x) == CHAR && TYPE (y) == CHAR
//               && VALUE (x) == VALUE (y))
//           || (TYPE (x) == NUMBER && TYPE (y) == NUMBER
//               && VALUE (x) == VALUE (y)))
//     ? cell_t : cell_f;
// }

SCM
gc_push_frame ()
{
  SCM frame = cons (r1, cons (r2, cons (r3, cons (r0, cell_nil))));
  g_stack = cons (frame, g_stack);
  return g_stack;
}

SCM
append2 (SCM x, SCM y)
{
  if (x == cell_nil) return y;
#if __GNUC__
  //FIXME GNUC
  assert (TYPE (x) == PAIR);
#endif
  return cons (car (x), append2 (cdr (x), y));
}

SCM
pairlis (SCM x, SCM y, SCM a)
{
  if (x == cell_nil)
    return a;
  if (TYPE (x) != PAIR)
    return cons (cons (x, y), a);
  return cons (cons (car (x), car (y)),
               pairlis (cdr (x), cdr (y), a));
}

SCM
assq (SCM x, SCM a)
{
  //while (a != cell_nil && eq_p (x, CAAR (a)) == cell_f) a = CDR (a);
  while (a != cell_nil && x == CAAR (a)) a = CDR (a);
  return a != cell_nil ? car (a) : cell_f;
}

#if __GNUC__
  //FIXME GNUC
SCM
assq_ref_env (SCM x, SCM a)
{
  x = assq (x, a);
  if (x == cell_f) return cell_undefined;
  return cdr (x);
}
#endif

#if __GNUC__
  //FIXME GNUC
SCM
assert_defined (SCM x, SCM e)
{
  if (e != cell_undefined) return e;
  // error (cell_symbol_unbound_variable, x);
  puts ("unbound variable");
  exit (33);
  return e;
}
#endif

SCM
push_cc (SCM p1, SCM p2, SCM a, SCM c) ///((internal))
{
  puts ("push cc\n");
  SCM x = r3;
  r3 = c;
  r2 = p2;
  gc_push_frame ();
  r1 = p1;
  r0 = a;
  r3 = x;
  return cell_unspecified;
}

#if __GNUC__
SCM caar (SCM x) {return car (car (x));}
SCM cadr (SCM x) {return car (cdr (x));}
SCM cdar (SCM x) {return cdr (car (x));}
SCM cddr (SCM x) {return cdr (cdr (x));}
#else
// Weirdness: wrong function labeling
// SCM cadr (SCM x) {
//   x = cdr (x);
//   return car (x);
// }
// SCM cddr (SCM x) {
//   x = cdr (x);
//   return cdr (x);
// }
#endif

#if __GNUC__
//FIXME
SCM call (SCM,SCM);
SCM gc_pop_frame ();
#endif

SCM
eval_apply ()
{
 eval_apply:
  // if (g_free + GC_SAFETY > ARENA_SIZE)
  //   gc_pop_frame (gc (gc_push_frame ()));

  switch (r3)
    {
    case cell_vm_apply: {goto apply;}
    case cell_unspecified: {return r1;}
    }

  SCM x = cell_nil;
  SCM y = cell_nil;

 apply:
  switch (TYPE (car (r1)))
    {
    case TFUNCTION: {
      puts ("apply.function\n");
      //check_formals (car (r1), MAKE_NUMBER (FUNCTION (car (r1)).arity), cdr (r1));
      r1 = call (car (r1), cdr (r1));
      goto vm_return;
    }
    }
 vm_return:
  x = r1;
  gc_pop_frame ();
  r1 = x;
  goto eval_apply;
}

SCM
call (SCM fn, SCM x)
{
  puts ("call\n");
#if __GNUC__
  //fn=11
  //function1
  puts ("fn=");
  puts (itoa(fn)); 
  puts ("\n");
  puts ("functiono");
  puts (itoa(g_cells[fn].cdr));
  puts ("\n");
#endif
  if (fn != 11) {
    puts("FN != 11\n");
    return 11;
  }
  if (g_cells[11].cdr != 1) {
    puts("fn.cdr != 11\n");
    return 11;
  }
  
  if ((FUNCTION (fn).arity > 0 || FUNCTION (fn).arity == -1)
      && x != cell_nil && TYPE (CAR (x)) == VALUES)
    x = cons (CADAR (x), CDR (x));
  puts ("00\n");
  if ((FUNCTION (fn).arity > 1 || FUNCTION (fn).arity == -1)
      && x != cell_nil && TYPE (CDR (x)) == PAIR && TYPE (CADR (x)) == VALUES)
    x = cons (CAR (x), cons (CDADAR (x), CDR (x)));
  //struct function* f = &FUNCTION (fn);
  puts ("01\n");
  switch (2)///FIXME FUNCTION (fn).arity)
    {
    // case 0: return FUNCTION (fn).function0 ();
    // case 1: return FUNCTION (fn).function1 (car (x));
    // case 2: return FUNCTION (fn).function2 (car (x), cadr (x));
    // case 3: return FUNCTION (fn).function3 (car (x), cadr (x), car (cddr (x)));
    // case -1: return FUNCTION (fn).functionn (x);
    case 0: {puts("02.0\n");return (FUNCTION (fn).function) ();}
    case 1: {puts("03.1\n");return ((SCM(*)(SCM))(FUNCTION (fn).function)) (car (x));}
#if 0
      //__GNUC__
    case 2: {return ((SCM(*)(SCM,SCM))(FUNCTION (fn).function)) (car (x), cadr (x));}
#else
    case 2: {
      puts ("04.2\n");
      SCM p1 = car (x);
      SCM p2 = cdr (x);
      p2 = car (p2);
      //return ((SCM(*)(SCM,SCM))(FUNCTION (fn).function)) (p1, p2);
      int (*functionx) (int,int) = (SCM(*)(SCM,SCM))FUNCTION (fn).function;
      //return ((SCM(*)(SCM,SCM))(*FUNCTION (fn).function)) (p1, p2);
      //return ((SCM(*)(SCM,SCM))(*functionx)) (p1, p2);
      SCM p3;
      //p3 = 0x44;
      puts ("05\n");
      return cons (p1, p2);
      return (*functionx) (p1, p2);
    }
#endif
    case 3: {puts("05.3\n");return ((SCM(*)(SCM,SCM,SCM))(FUNCTION (fn).function)) (car (x), cadr (x), car (cddr (x)));}
      //case -1: {return ((SCM(*)(SCM))(FUNCTION (fn).function)) (x);}
    default: {return ((SCM(*)(SCM))(FUNCTION (fn).function)) (x);}
    }

  return cell_unspecified;
}

SCM
gc_peek_frame ()
{
  SCM frame = car (g_stack);
  r1 = car (frame);
#if __GNUC__
  r2 = cadr (frame);
  r3 = car (cddr (frame));
  r0 = cadr (cddr (frame));
#else
  r2 = cdr (frame);
  r2 = car (r2);

  r3 = cdr (frame);
  r3 = cdr (r3);
  r3 = car (r3);

  r0 = cdr (frame);
  r0 = cdr (r0);
  r0 = cdr (r0);
  r0 = cdr (r0);
  r0 = car (r0);
#endif
  return frame;
}

SCM
gc_pop_frame ()
{
  SCM frame = gc_peek_frame (g_stack);
  g_stack = cdr (g_stack);
  return frame;
}

SCM
mes_g_stack (SCM a) ///((internal))
{
  r0 = a;
  r1 = MAKE_CHAR (0);
  r2 = MAKE_CHAR (0);
  r3 = MAKE_CHAR (0);
  g_stack = cons (cell_nil, cell_nil);
  return r0;
}

// Environment setup
SCM
make_tmps (struct scm* cells)
{
  tmp = g_free++;
  cells[tmp].type = CHAR;
  tmp_num = g_free++;
  cells[tmp_num].type = NUMBER;
  tmp_num2 = g_free++;
  cells[tmp_num2].type = NUMBER;
  return 0;
}

SCM
make_symbol_ (SCM s)
{
  VALUE (tmp_num) = SYMBOL;
  SCM x = make_cell (tmp_num, s, 0);
  g_symbols = cons (x, g_symbols);
  return x;
}

SCM
make_symbol (SCM s)
{
#if MES_MINI
  SCM x = 0;
#else
  SCM x = lookup_symbol_ (s);
#endif
  return x ? x : make_symbol_ (s);
}

SCM
cstring_to_list (char const* s)
{
  SCM p = cell_nil;
  int i = strlen (s);
  while (i--)
    p = cons (MAKE_CHAR (s[i]), p);
  return p;
}

SCM
acons (SCM key, SCM value, SCM alist)
{
  return cons (cons (key, value), alist);
}

// Jam Collector
SCM g_symbol_max;

SCM
gc_init_cells ()
{
  return 0;
//   g_cells = (scm *)malloc (2*ARENA_SIZE*sizeof(scm));

// #if __NYACC__ || FIXME_NYACC
//   TYPE (0) = TVECTOR;
// // #else
// //   TYPE (0) = VECTOR;
// #endif
//   LENGTH (0) = 1000;
//   VECTOR (0) = 0;
//   g_cells++;
//   TYPE (0) = CHAR;
//   VALUE (0) = 'c';
}

// INIT NEWS

SCM
mes_symbols () ///((internal))
{
  gc_init_cells ();
  //  gc_init_news ();

#if __GNUC__ && 0
  //#include "mes.symbols.i"
#else
g_free++;
// g_cells[cell_nil] = scm_nil;

g_free++;
// g_cells[cell_f] = scm_f;

g_free++;
// g_cells[cell_t] = scm_t;

g_free++;
// g_cells[cell_dot] = scm_dot;

g_free++;
// g_cells[cell_arrow] = scm_arrow;

g_free++;
// g_cells[cell_undefined] = scm_undefined;

g_free++;
// g_cells[cell_unspecified] = scm_unspecified;

g_free++;
// g_cells[cell_closure] = scm_closure;

g_free++;
// g_cells[cell_circular] = scm_circular;

g_free++;
// g_cells[cell_begin] = scm_begin;

///
g_free = 44;
g_free++;
// g_cells[cell_vm_apply] = scm_vm_apply;

g_free++;
// g_cells[cell_vm_apply2] = scm_vm_apply2;

g_free++;
// g_cells[cell_vm_eval] = scm_vm_eval;

///
g_free = 55;
g_free++;
// g_cells[cell_vm_begin] = scm_vm_begin;

g_free++;
// g_cells[cell_vm_begin_read_input_file] = scm_vm_begin_read_input_file;

g_free++;
// g_cells[cell_vm_begin2] = scm_vm_begin2;

///
g_free = 62;
g_free++;
// g_cells[cell_vm_return] = scm_vm_return;

#endif

  g_symbol_max = g_free;
  make_tmps (g_cells);

  g_symbols = 0;
  for (int i=1; i<g_symbol_max; i++)
    g_symbols = cons (i, g_symbols);

  SCM a = cell_nil;

#if __GNUC__ && 0
  //#include "mes.symbol-names.i"
#else
// g_cells[cell_nil].car = cstring_to_list (scm_nil.name);
// g_cells[cell_f].car = cstring_to_list (scm_f.name);
// g_cells[cell_t].car = cstring_to_list (scm_t.name);
// g_cells[cell_dot].car = cstring_to_list (scm_dot.name);
// g_cells[cell_arrow].car = cstring_to_list (scm_arrow.name);
// g_cells[cell_undefined].car = cstring_to_list (scm_undefined.name);
// g_cells[cell_unspecified].car = cstring_to_list (scm_unspecified.name);
// g_cells[cell_closure].car = cstring_to_list (scm_closure.name);
// g_cells[cell_circular].car = cstring_to_list (scm_circular.name);
// g_cells[cell_begin].car = cstring_to_list (scm_begin.name);
#endif

  // a = acons (cell_symbol_mes_version, MAKE_STRING (cstring_to_list (VERSION)), a);
  // a = acons (cell_symbol_mes_prefix, MAKE_STRING (cstring_to_list (PREFIX)), a);

  a = acons (cell_symbol_dot, cell_dot, a);
  a = acons (cell_symbol_begin, cell_begin, a);
  a = acons (cell_closure, a, a);

  // a = acons (cell_symbol_call_with_current_continuation, cell_call_with_current_continuation, a);
  // a = acons (cell_symbol_sc_expand, cell_f, a);

  return a;
}

SCM
make_closure (SCM args, SCM body, SCM a)
{
  return make_cell (tmp_num_ (CLOSURE), cell_f, cons (cons (cell_circular, a), cons (args, body)));
}

SCM
mes_environment () ///((internal))
{
  SCM a = 0;
  a = mes_symbols ();
  a = mes_g_stack (a);
  return a;
}

SCM
mes_builtins (SCM a)
{
#if 0
  //__GNUC__
//#include "mes.i"

// #include "lib.i"
// #include "math.i"
// #include "posix.i"
// #include "reader.i"

// #include "lib.environment.i"
// #include "math.environment.i"
// #include "mes.environment.i"
// #include "posix.environment.i"
// #include "reader.environment.i"
#else

scm_make_cell.cdr = g_function;
functions[g_function++] = fun_make_cell;
cell_make_cell = g_free++;
#if __GNUC__
 puts ("WOOOT=");
 puts (itoa (g_free));
 puts ("\n");
  //FIXME GNUC
 g_cells[cell_make_cell] = scm_make_cell;
#else
g_cells[16] = scm_make_cell;
#endif
 
scm_cons.cdr = g_function;
functions[g_function++] = fun_cons;
cell_cons = g_free++;
#if __GNUC__
  //FIXME GNUC
g_cells[cell_cons] = scm_cons;
#else
g_cells[17] = scm_cons;
#endif
 
scm_car.cdr = g_function;
functions[g_function++] = fun_car;
cell_car = g_free++;
#if __GNUC__
  //FIXME GNUC
g_cells[cell_car] = scm_car;
#endif
 
#if __GNUC__
  //FIXME GNUC
scm_cdr.cdr = g_function;
functions[g_function++] = fun_cdr;
cell_cdr = g_free++;
g_cells[cell_cdr] = scm_cdr;

// scm_make_cell.string = cstring_to_list (scm_make_cell.name);
// g_cells[cell_make_cell].string = MAKE_STRING (scm_make_cell.string);
// a = acons (make_symbol (scm_make_cell.string), cell_make_cell, a);

// scm_cons.string = cstring_to_list (scm_cons.name);
// g_cells[cell_cons].string = MAKE_STRING (scm_cons.string);
// a = acons (make_symbol (scm_cons.string), cell_cons, a);

// scm_car.string = cstring_to_list (scm_car.name);
// g_cells[cell_car].string = MAKE_STRING (scm_car.string);
// a = acons (make_symbol (scm_car.string), cell_car, a);

// scm_cdr.string = cstring_to_list (scm_cdr.name);
// g_cells[cell_cdr].string = MAKE_STRING (scm_cdr.string);
// a = acons (make_symbol (scm_cdr.string), cell_cdr, a);
#endif
#endif
  return a;
}

SCM
bload_env (SCM a) ///((internal))
{
  g_stdin = open ("module/mes/read-0.mo", 0);
#if __GNUC__
  //g_stdin = g_stdin ? g_stdin : fopen (PREFIX "module/mes/read-0.mo", "r");
#endif
  char *p = (char*)g_cells;
#if __GNUC__
  //FIXME GNUC
  assert (getchar () == 'M');
  assert (getchar () == 'E');
  assert (getchar () == 'S');
#else
  getchar ();
  getchar ();
  getchar ();
#endif
  g_stack = getchar () << 8;
  g_stack += getchar ();
  int c = getchar ();
  while (c != EOF)
    {
      *p++ = c;
      c = getchar ();
    }
  g_free = (p-(char*)g_cells) / sizeof (struct scm);
  gc_peek_frame ();
  g_symbols = r1;
  g_stdin = STDIN;
  r0 = mes_builtins (r0);
  return r2;
}

SCM
fill ()
{
  TYPE (0) = 0x6c6c6168;
  CAR (0) = 0x6a746f6f;
  CDR (0) = 0x00002165;

  TYPE (1) = SYMBOL;
  CAR (1) = 0x2d2d2d2d;
  CDR (1) = 0x3e3e3e3e;

  TYPE (9) = 0x2d2d2d2d;
  CAR (9) = 0x2d2d2d2d;
  CDR (9) = 0x3e3e3e3e;
#if 0
  // (A(B))
  TYPE (10) = PAIR;
  CAR (10) = 11;
  CDR (10) = 12;

  TYPE (11) = CHAR;
  CAR (11) = 0x58585858;
  CDR (11) = 89;

  TYPE (12) = PAIR;
  CAR (12) = 13;
  CDR (12) = 1;

  TYPE (13) = CHAR;
  CAR (13) = 0x58585858;
  CDR (13) = 90;

  TYPE (14) = 0x58585858;
  CAR (14) = 0x58585858;
  CDR (14) = 0x58585858;

  TYPE (14) = 0x58585858;
  CAR (14) = 0x58585858;
  CDR (14) = 0x58585858;
#else
  // (cons 0 1)
  TYPE (10) = PAIR;
  CAR (10) = 11;
  CDR (10) = 12;

  TYPE (11) = TFUNCTION;
  CAR (11) = 0x58585858;
  // 0 = make_cell
  // 1 = cons
  // 2 = car
  CDR (11) = 1;

  TYPE (12) = PAIR;
  CAR (12) = 13;
  //CDR (12) = 1;
  CDR (12) = 14;

  TYPE (13) = NUMBER;
  CAR (13) = 0x58585858;
  CDR (13) = 0;

  TYPE (14) = PAIR;
  CAR (14) = 15;
  CDR (14) = 1;

  TYPE (15) = NUMBER;
  CAR (15) = 0x58585858;
  CDR (15) = 1;

  //g_stack@23
  TYPE (19) = PAIR;
  CAR (19) = 1;
  CDR (19) = 1;

  TYPE (20) = PAIR;
  CAR (20) = 7;
  CDR (20) = 19;

  TYPE (21) = PAIR;
  CAR (21) = 7;
  CDR (21) = 20;

  TYPE (22) = PAIR;
  CAR (22) = 134;
  CDR (22) = 21;

  TYPE (23) = PAIR;
  CAR (23) = 22;
  CDR (23) = 137;

#endif

  return 0;
}

SCM
display_ (SCM x)
{
  //puts ("<display>\n");
  switch (TYPE (x))
    {
    case CHAR:
      {
        //puts ("<char>\n");
        puts ("#\\");
        putchar (VALUE (x));
        break;
      }
    case TFUNCTION:
      {
        //puts ("<function>\n");
        if (VALUE (x) == 0)
          puts ("make-cell");
        if (VALUE (x) == 1)
          puts ("cons");
        if (VALUE (x) == 2)
          puts ("car");
        if (VALUE (x) == 3)
          puts ("cdr");
        break;
      }
    case NUMBER:
      {
        //puts ("<number>\n");
#if __GNUC__
        putchar (48 + VALUE (x));
#else
        int i;
        i = VALUE (x);
        i = i + 48;
        putchar (i);
#endif
        break;
      }
    case PAIR:
      {
        //puts ("<pair>\n");
        //if (cont != cell_f) puts "(");
        puts ("(");
        if (x && x != cell_nil) display_ (CAR (x));
        if (CDR (x) && CDR (x) != cell_nil)
          {
#if __GNUC__
            if (TYPE (CDR (x)) != PAIR)
              puts (" . ");
#else
            int c;
            c = CDR (x);
            c = TYPE (c);
            if (c != PAIR)
              puts (" . ");
#endif
            display_ (CDR (x));
          }
        //if (cont != cell_f) puts (")");
        puts (")");
        break;
      }
    default:
      {
        //puts ("<default>\n");
        puts ("_");
        break;
      }
    }
  return 0;
}

SCM
simple_bload_env (SCM a) ///((internal))
{
  puts ("reading: ");
  char *mo = "module/mes/hack-32.mo";
  puts (mo);
  puts ("\n");
  g_stdin = open (mo, 0);
  if (g_stdin < 0) {eputs ("no such file: module/mes/read-0-32.mo\n");return 1;} 

  char *p = (char*)g_cells;
  int c;

#if 0
  //__GNUC__
  puts ("fd: ");
  puts (itoa (g_stdin));
  puts ("\n");
#endif

#if 0
  //__GNUC__
  assert (getchar () == 'M');
  assert (getchar () == 'E');
  assert (getchar () == 'S');
  puts (" *GOT MES*\n");
  g_stack = getchar () << 8;
  g_stack += getchar ();
  puts ("stack: ");
  puts (itoa (g_stack));
  puts ("\n");
#else
  c = getchar ();
  putchar (c);
  if (c != 'M') exit (10);
  c = getchar ();
  putchar (c);
  if (c != 'E') exit (11);
  c = getchar ();
  putchar (c);
  if (c != 'S') exit (12);
  puts (" *GOT MES*\n");

  // skip stack
  getchar ();
  getchar ();
#endif

  c = getchar ();
  while (c != -1)
    {
      *p++ = c;
      c = getchar ();
      putchar (c);
    }

  puts ("read done\n");

  // g_free = (p-(char*)g_cells) / sizeof (struct scm);
  c = p-(char*)g_cells;
  exit (c);
  
  
  
  
 if (g_free != 15) exit (33);
  
  // puts ("Xg_free: ");
  // puts (itoa (g_free));
  // puts ("\n");


  ///if (g_free != 19) return 33;
  
  // gc_peek_frame ();
  // g_symbols = r1;
  g_symbols = 1;
  g_stdin = STDIN;
  r0 = mes_builtins (r0);
  
#if __GNUC__
  puts ("cells read: ");
  puts (itoa (g_free));
  puts ("\n");

  puts ("symbols: ");
  puts (itoa (g_symbols));
  puts ("\n");
  // display_ (g_symbols);
  // puts ("\n");
#endif

  display_ (10);
  puts ("\n");

  fill ();
  r2 = 10;

  if (TYPE (12) != PAIR)
    exit (33);

  puts ("program[");
#if __GNUC__
  puts (itoa (r2));
#endif
  puts ("]: ");

  display_ (r2);
  //display_ (14);
  puts ("\n");

  r0 = 1;
  //r2 = 10;
  return r2;
}

char const*
string_to_cstring (SCM s)
{
  static char buf[1024];
  char *p = buf;
  s = STRING(s);
  while (s != cell_nil)
    {
      *p++ = VALUE (car (s));
      s = cdr (s);
    }
  *p = 0;
  return buf;
}

SCM
stderr_ (SCM x)
{
  //SCM write;
#if __NYACC__ || FIXME_NYACC
  if (TYPE (x) == TSTRING)
// #else
//   if (TYPE (x) == STRING)
#endif
    eputs (string_to_cstring (x));
  // else if ((write = assq_ref_env (cell_symbol_write, r0)) != cell_undefined)
  //   apply (assq_ref_env (cell_symbol_display, r0), cons (x, cons (MAKE_NUMBER (2), cell_nil)), r0);
#if __NYACC__ || FIXME_NYACC
  else if (TYPE (x) == SPECIAL || TYPE (x) == TSTRING || TYPE (x) == SYMBOL)
// #else
//   else if (TYPE (x) == SPECIAL || TYPE (x) == STRING || TYPE (x) == SYMBOL)
#endif
    eputs (string_to_cstring (x));
  else if (TYPE (x) == NUMBER)
    eputs (itoa (VALUE (x)));
  else
    eputs ("display: undefined\n");
  return cell_unspecified;
}

int
main (int argc, char *argv[])
{
  puts ("Hello mini-mes!\n");
#if __GNUC__
  //g_debug = getenv ("MES_DEBUG");
#endif
  //if (getenv ("MES_ARENA")) ARENA_SIZE = atoi (getenv ("MES_ARENA"));
  if (argc > 1 && !strcmp (argv[1], "--help")) return eputs ("Usage: mes [--dump|--load] < FILE");
#if __GNUC__
  if (argc > 1 && !strcmp (argv[1], "--version")) {eputs ("Mes ");return eputs (VERSION);};
#else
  if (argc > 1 && !strcmp (argv[1], "--version")) {eputs ("Mes ");return eputs ("0.4");};
#endif
  g_stdin = STDIN;

  r0 = mes_environment ();
  
#if MES_MINI
  SCM program = simple_bload_env (r0);
#else  
  SCM program = (argc > 1 && !strcmp (argv[1], "--load"))
    ? bload_env (r0) : load_env (r0);
  if (argc > 1 && !strcmp (argv[1], "--dump")) return dump ();
#endif

#if 1

#if __GNUC__
  puts ("g_free=");
  puts (itoa(g_free));
  puts ("\n");
#else
  g_free = 19;

#endif

  //return cons (r0, cell_nil);

  //FIXME
  push_cc (r2, cell_unspecified, r0, cell_unspecified);
#if __GNUC__
  for (int x=19; x<26 ;x++)
    {
      puts(itoa(x));
      puts(": type=");
      puts(itoa(g_cells[x].type));
      puts(" car=");
      puts(itoa(g_cells[x].car));
      puts(" cdr=");
      puts(itoa(g_cells[x].cdr));
      puts("\n");
    }
#endif
#else
  g_stack = 23;
  g_free = 24;
  r1 = r2; //10: the-program
  r2 = cell_unspecified;
#endif

  puts ("g_stack: ");
  display_ (g_stack);
  puts ("\n");

#if __GNUC__

  puts ("g_free=");
  puts (itoa(g_free));
  puts ("\n");

  puts ("g_stack=");
  puts (itoa(g_stack));
  puts ("\n");

  puts ("r0=");
  puts (itoa(r0));
  puts ("\n");

  puts ("r1=");
  puts (itoa(r1));
  puts ("\n");

  puts ("r2=");
  puts (itoa(r2));
  puts ("\n");

  puts ("r3=");
  puts (itoa(r3));
  puts ("\n");
#endif

  //r3 = cell_vm_begin;
  r3 = cell_vm_apply;
  r1 = eval_apply ();
  //stderr_ (r1);
  display_ (r1);

  eputs ("\n");
#if !MES_MINI
  gc (g_stack);
#endif
#if __GNUC__
  if (g_debug)
    {
      eputs ("\nstats: [");
      eputs (itoa (g_free));
      eputs ("]\n");
    }
#endif
  return 0;
}

#if __GNUC__
void
_start ()
{
  int r;
  asm (
       "mov %%ebp,%%eax\n\t"
       "addl $8,%%eax\n\t"
       "push %%eax\n\t"

       "mov %%ebp,%%eax\n\t"
       "addl $4,%%eax\n\t"
       "movzbl (%%eax),%%eax\n\t"
       "push %%eax\n\t"

       "call main\n\t"
       "movl %%eax,%0\n\t"
       : "=r" (r)
       : //no inputs "" (&main)
       );
  exit (r);
}
#endif
