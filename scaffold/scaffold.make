TARGET:=m
C_FILES:=$(DIR)/m.c
DEFINES:=POSIX=1
INCLUDES:=libc
include make/bin.make

TARGET:=m
EXPECT:=255
include make/check.make

TARGET:=hello
C_FILES:=$(DIR)/hello.c
DEFINES:=POSIX=1
INCLUDES:=libc
include make/bin.make

TARGET:=hello
EXPECT:=42
include make/check.make

TARGET:=t
C_FILES:=$(DIR)/t.c
DEFINES:=POSIX=1
INCLUDES:=libc
include make/bin.make

TARGET:=t
include make/check.make

TARGET:=m.mlibc
C_FILES:=$(DIR)/m.c
include make/bin-mlibc.make

TARGET:=m.mlibc
EXPECT:=255
include make/check.make

TARGET:=hello.mlibc
C_FILES:=$(DIR)/hello.c
include make/bin-mlibc.make

TARGET:=hello.mlibc
EXPECT:=42
include make/check.make

TARGET:=micro-mes.mlibc
C_FILES:=$(DIR)/micro-mes.c
include make/bin-mlibc.make

TEST:=micro-mes.mlibc-check
$(TEST): $(OUT)/micro-mes.mlibc
	$< 2 3; r=$$?; [ $$r = 3 ]
include make/check.make

TARGET:=tiny-mes.mlibc
C_FILES:=$(DIR)/tiny-mes.c
include make/bin-mlibc.make

TARGET:=tiny-mes.mlibc
include make/check.make

TARGET:=cons-mes.mlibc
C_FILES:=$(DIR)/cons-mes.c
DEFINES:=VERSION='"$(VERSION)"'
include make/bin-mlibc.make

TARGET:=cons-mes.mlibc
include make/check.make

TARGET:=t.mlibc
C_FILES:=$(DIR)/t.c
include make/bin-mlibc.make

TARGET:=t.mlibc
include make/check.make

CROSS:=$(CC32:%gcc=%)
#$(OUT)/$(DIR)/mini-mes.$(CROSS)o: $(SNARF.MES)
$(OUT)/mini-mes: $(SNARF.MES)

TARGET:=mini-mes.mlibc
C_FILES:=$(DIR)/mini-mes.c
DEFINES:=FIXED_PRIMITIVES=1 VERSION='"$(VERSION)"' MODULEDIR='"$(MODULEDIR)"' PREFIX='"$(PREFIX)"'
INCLUDES:=src $(OUT)/src
include make/bin-mlibc.make

TEST:=mini-mes.mlibc-check
$(TEST): $(OUT)/mini-mes.mlibc
	echo 0 | $<
include make/check.make

# guile/mescc.scm

TARGET:=m.guile
C_FILES:=$(DIR)/m.c
include make/mescc-guile.make

TARGET:=m.guile
EXPECT:=255
include make/check.make

TARGET:=hello.guile
C_FILES:=$(DIR)/hello.c
include make/mescc-guile.make

TARGET:=hello.guile
EXPECT:=42
include make/check.make

TARGET:=micro-mes.guile
C_FILES:=$(DIR)/micro-mes.c
include make/mescc-guile.make

TEST:=micro-mes.guile-check
$(TEST): $(OUT)/micro-mes.guile
	$< 2 3; r=$$?; [ $$r = 3 ]
include make/check.make

$(OUT)/tiny-mes.mes: module/mes/tiny-0-32.mo
TARGET:=tiny-mes.guile
C_FILES:=$(DIR)/tiny-mes.c
include make/mescc-guile.make

TARGET:=tiny-mes.guile
include make/check.make

TARGET:=cons-mes.guile
C_FILES:=$(DIR)/cons-mes.c
include make/mescc-guile.make

TARGET:=cons-mes.guile
include make/check.make

TARGET:=t.guile
C_FILES:=$(DIR)/t.c
include make/mescc-guile.make

TARGET:=t.guile
include make/check.make

$(OUT)/mini-mes.guile: module/mes/read-0-32.mo
TARGET:=mini-mes.guile
C_FILES:=$(DIR)/mini-mes.c
include make/mescc-guile.make

TEST:=mini-mes.guile-check
$(TEST): $(OUT)/mini-mes.guile
	echo 0 | $<
include make/check.make

# scripts/mescc.mes
ifeq ($(MES_SKIP_MES),)
TARGET:=m.mes
C_FILES:=$(DIR)/m.c
include make/mescc-mes.make

TARGET:=m.mes
EXPECT:=255
include make/check.make

ifneq ($(SCAFFOLD),)
TARGET:=hello.mes
C_FILES:=$(DIR)/hello.c
include make/mescc-mes.make

TARGET:=hello.mes
EXPECT:=42
include make/check.make

TARGET:=micro-mes.mes
C_FILES:=$(DIR)/micro-mes.c
include make/mescc-mes.make

TEST:=micro-mes.mes-check
$(TEST): $(OUT)/micro-mes.mes
	$< 2 3; r=$$?; [ $$r = 3 ]
include make/check.make

$(OUT)/tiny-mes.mes: module/mes/tiny-0-32.mo
TARGET:=tiny-mes.mes
C_FILES:=$(DIR)/tiny-mes.c
include make/mescc-mes.make

TARGET:=tiny-mes.mes
include make/check.make

TARGET:=cons-mes.mes
C_FILES:=$(DIR)/cons-mes.c
include make/mescc-mes.make

TARGET:=cons-mes.mes
include make/check.make
endif # !SCAFFOLD

TARGET:=t.mes
C_FILES:=$(DIR)/t.c
include make/mescc-mes.make

TARGET:=t.mes
include make/check.make
endif

ifneq ($(BOOTSTRAP),)
$(OUT)/mini-mes.mes: module/mes/read-0-32.mo
TARGET:=mini-mes.mes
C_FILES:=$(DIR)/mini-mes.c
include make/mescc-mes.make
endif
