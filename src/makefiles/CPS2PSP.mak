#------------------------------------------------------------------------------
#
#                             CPS2PSP Makefile
#
#------------------------------------------------------------------------------

PSP_EBOOT_ICON = data/cps2.png

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
	src/cps2


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
# Object Files
#------------------------------------------------------------------------------

COREOBJS = \
	cps2/cps2.o \
	cps2/cps2crpt.o \
	cps2/driver.o \
	cps2/memintrf.o \
	cps2/inptport.o \
	cps2/timer.o \
	cps2/vidhrdw.o \
	cps2/sprite.o \
	cps2/eeprom.o \
	sound/qsound.o

ICONOBJS = \
	$(OS)/icon/cps_s.o \
	$(OS)/icon/cps_l.o
