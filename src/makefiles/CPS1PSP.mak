#------------------------------------------------------------------------------
#
#                             CPS1PSP Makefile
#
#------------------------------------------------------------------------------

PSP_EBOOT_ICON = data/cps1.png

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
	src/cps1


#------------------------------------------------------------------------------
# Object Directory
#------------------------------------------------------------------------------

OBJDIRS += \
	cpu/m68000 \
	cpu/z80 \
	cps1


#------------------------------------------------------------------------------
# Object Files (common)
#------------------------------------------------------------------------------

MAINOBJS += \
	cpu/m68000/m68000.o \
	cpu/m68000/c68k.o \
	cpu/z80/z80.o \
	cpu/z80/cz80.o \
	common/coin.o


#------------------------------------------------------------------------------
# Object Files (CPS1PSP)
#------------------------------------------------------------------------------

COREOBJS = \
	cps1/cps1.o \
	cps1/driver.o \
	cps1/memintrf.o \
	cps1/inptport.o \
	cps1/dipsw.o \
	cps1/timer.o \
	cps1/vidhrdw.o \
	cps1/sprite.o \
	cps1/eeprom.o \
	cps1/kabuki.o \
	sound/2151intf.o \
	sound/ym2151.o \
	sound/qsound.o

ICONOBJS = \
	$(OS)/icon/cps_s.o \
	$(OS)/icon/cps_l.o
