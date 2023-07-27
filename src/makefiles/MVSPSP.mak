#------------------------------------------------------------------------------
#
#                             MVSPSP Makefile
#
#------------------------------------------------------------------------------

PSP_EBOOT_ICON = data/mvs.png

ifdef LARGE_MEMORY
PSP_EBOOT_TITLE = $(PBPNAME_STR) $(VERSION_STR) for PSP slim
else
ifdef KERNEL_MODE
PSP_EBOOT_TITLE = $(PBPNAME_STR) $(VERSION_STR)
else
PSP_EBOOT_TITLE = $(PBPNAME_STR) $(VERSION_STR) for PSP
endif
endif

#------------------------------------------------------------------------------
# File include path
#------------------------------------------------------------------------------

INCDIR += \
	src/cpu/m68000 \
	src/cpu/z80 \
	src/mvs


#------------------------------------------------------------------------------
# Object Directory
#------------------------------------------------------------------------------

OBJDIRS += \
	cpu/m68000 \
	cpu/z80 \
	mvs


#------------------------------------------------------------------------------
# Object Files (common)
#------------------------------------------------------------------------------

MAINOBJS += \
	cpu/m68000/m68000.o \
	cpu/m68000/c68k.o \
	cpu/z80/z80.o \
	cpu/z80/cz80.o


#------------------------------------------------------------------------------
# Object Files
#------------------------------------------------------------------------------

COREOBJS = \
	mvs/mvs.o \
	mvs/driver.o \
	mvs/memintrf.o \
	mvs/inptport.o \
	mvs/dipsw.o \
	mvs/timer.o \
	mvs/vidhrdw.o \
	mvs/sprite.o \
	mvs/pd4990a.o \
	mvs/neocrypt.o \
	mvs/biosmenu.o \
	sound/2610intf.o \
	sound/ym2610.o

ICONOBJS = \
	$(OS)/icon/mvs_s.o \
	$(OS)/icon/mvs_l.o
