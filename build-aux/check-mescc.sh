#! /bin/sh

# Mes --- Maxwell Equations of Software
# Copyright © 2017,2018 Jan (janneke) Nieuwenhuizen <janneke@gnu.org>
#
# This file is part of Mes.
#
# Mes is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or (at
# your option) any later version.
#
# Mes is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Mes.  If not, see <http://www.gnu.org/licenses/>.

export MES=${MES-src/mes}
export MESCC=${MESCC-scripts/mescc}
export GUILE=${GUILE-guile}
export MES_PREFIX=${MES_PREFIX-.}

export HEX2=${HEX2-hex2}
export M1=${M1-M1}
export BLOOD_ELF=${BLOOD_ELF-blood-elf}
export MES_SEED=${MES_SEED-../mes-seed}
export MESCC=${MESCC-$(type -p mescc)}
[ -z "$MESCC" ] && MESCC=scripts/mescc
export MES=${MES-$(type -p mes)}
[ -z "$MES" ] && MES=src/mes


tests="
t
00-exit-0
01-return-0
02-return-1
03-call
04-call-0
05-call-1
06-call-!1
10-if-0
11-if-1
12-if-==
13-if-!=
14-if-goto
15-if-!f
16-if-t
20-while
21-char[]
22-while-char[]
23-pointer
30-strlen
31-eputs
32-compare
33-and-or
34-pre-post
35-compare-char
36-compare-arithmetic
37-compare-assign
38-compare-call
40-if-else
41-?
42-goto-label
43-for-do-while
44-switch
45-void-call
50-assert
51-strcmp
52-itoa
53-strcpy
54-argv
60-math
61-array
63-struct-cell
64-make-cell
65-read
70-printf
71-struct-array
72-typedef-struct-def
73-union
74-multi-line-string
75-struct-union
76-pointer-arithmetic
77-pointer-assign
78-union-struct
79-int-array
7a-struct-char-array
7b-struct-int-array
7c-dynarray
7d-cast-char
7e-struct-array-access
7f-struct-pointer-arithmetic
7g-struct-byte-word-field
7h-struct-assign
7i-struct-struct
7j-strtoull
7k-for-each-elem
7l-struct-any-size-array
7m-struct-char-array-assign
7n-struct-struct-array
80-setjmp
81-qsort
82-define
"

broken="$broken
"

set +e
expect=$(echo $broken | wc -w)
pass=0
fail=0
total=0
export LIBC=libc/libc
for t in $tests; do
    if [ -z "${t/[012][0-9]-*/}" ]; then
        LIBC=lib/mini-libc;
    elif [ -z "${t/8[0-9]-*/}" ]; then
        LIBC=lib/libc+tcc;
    else
        LIBC=lib/libc;
    fi
    sh build-aux/test.sh "scaffold/tests/$t" &> scaffold/tests/"$t".log
    r=$?
    total=$((total+1))
    if [ $r = 0 ]; then
        echo $t: [OK]
        pass=$((pass+1))
    else
        echo $t: [FAIL]
        fail=$((fail+1))
    fi
done

tests="
00_assignment
01_comment
02_printf
03_struct
04_for
05_array
06_case
07_function
08_while
09_do_while

10_pointer
11_precedence
12_hashdefine
13_integer_literals
14_if
15_recursion
16_nesting
17_enum
18_include
19_pointer_arithmetic

20_pointer_comparison
21_char_array
22_floating_point
23_type_coercion
24_math_library
25_quicksort
26_character_constants
27_sizeof
28_strings
29_array_address

30_hanoi
31_args
32_led
33_ternary_op
34_array_assignment
35_sizeof
36_array_initialisers
37_sprintf
38_multiple_array_index
39_typedef

40_stdio
41_hashif
42_function_pointer
43_void_param
44_scoped_declarations
45_empty_for
46_grep
47_switch_return
48_nested_break
49_bracket_evaluation

50_logical_second_arg
51_static
52_unnamed_enum
55_lshift_type
54_goto
"

broken="$broken
18_include

22_floating_point
23_type_coercion
24_math_library
26_character_constants
27_sizeof
28_strings

30_hanoi
32_led
34_array_assignment
37_sprintf
38_multiple_array_index
39_typedef

40_stdio
42_function_pointer
46_grep
49_bracket_evaluation

51_static
52_unnamed_enum
55_lshift_type
"

#22_floating_point       ; float
#23_type_coercion        ; float
#24_math_library         ; float
#27_sizeof               ; float
#28_strings              ; TODO: strncpy strchr strrchr memset memcpy memcmp
#30_hanoi                ; fails with GCC
#32_led                  ; unsupported: (decl (decl-spec-list (stor-spec (static)) (type-spec (fixed-type "int"))) (init-declr-list (init-declr (array-of (ident "d") (p-expr (fixed "32"))))))
#34_array_assignment     ; fails with GCC
#36_array_initialisers   ; unspported: (decl (decl-spec-list (type-spec (fixed-type "int"))) (init-declr-list (init-declr (array-of (ident "Array") (p-expr (fixed "10"))) (initzer (initzer-list (initzer (p-expr (fixed "12"))) (initzer (p-expr (fixed "34"))) (initzer (p-expr (fixed "56"))) (initzer (p-expr (fixed "78"))) (initzer (p-expr (fixed "90"))) (initzer (p-expr (fixed "123"))) (initzer (p-expr (fixed "456"))) (initzer (p-expr (fixed "789"))) (initzer (p-expr (fixed "8642"))) (initzer (p-expr (fixed "9753"))))))))
#37_sprintf              ; integer formatting unsupported
#38_multiple_array_index ; unspported: (decl (decl-spec-list (type-spec (fixed-type "int"))) (init-declr-list (init-declr (array-of (array-of (ident "a") (p-expr (fixed "4"))) (p-expr (fixed "4"))))))
#39_typedef              ;unsupported: (decl (decl-spec-list (stor-spec (typedef)) (type-spec (typename "MyFunStruct"))) (init-declr-list (init-declr (ptr-declr (pointer) (ident "MoreFunThanEver")))))

#40_stdio                ; f* functions
#42_function_pointer     ; f* functions
#46_grep                 ; f* functions
#49_bracket_evaluation   ; float
#51_static               ; unsupported: (decl (decl-spec-list (stor-spec (static)) (type-spec (fixed-type "int"))) (init-declr-list (init-declr (ident "fred") (initzer (p-expr (fixed "1234"))))))
#52_unnamed_enum         ; unsupported: (decl (decl-spec-list (stor-spec (typedef)) (type-spec (enum-def (enum-def-list (enum-defn (ident "e")) (enum-defn (ident "f")) (enum-defn (ident "g")))))) (init-declr-list (init-declr (ident "h"))))
#55_lshift_type          ; unsigned


expect=$(echo $broken | wc -w)
for t in $tests; do
    if [ ! -f scaffold/tinycc/"$t.c" ]; then
        echo ' [SKIP]'
        continue;
    fi
    sh build-aux/test.sh "scaffold/tinycc/$t" arg1 arg2 arg3 arg4 arg5 &> scaffold/tinycc/"$t".log
    r=$?
    total=$((total+1))
    if [ $r = 0 ]; then
        echo $t: [OK]
        pass=$((pass+1))
    else
        echo $t: [FAIL]
        fail=$((fail+1))
    fi
done
[ $expect != 0 ] && echo "expect: $expect"
[ $fail != 0 ] && echo "failed: $fail"
[ $fail -lt $expect ] && echo "solved: $(($expect - $fail))"
echo "passed: $pass"
echo "total:  $total"
if [ $fail != 0 -a $fail -gt $expect ]; then
    echo FAILED: $fail/$total
    exit 1
elif [ $fail != 0 ]; then
    echo PASS: $pass/$total
else
    echo PASS: $total
fi
