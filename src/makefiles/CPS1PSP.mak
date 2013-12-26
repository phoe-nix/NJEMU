#------------------------------------------------------------------------------
#
#                             CPS1PSP Makefile
#
#------------------------------------------------------------------------------

PSP_EBOOT_ICON = data/cps1.png

ifdef PSP_SLIM
PSP_EBOOT_TITLE = $(PBPNAME_STR) $(VERSION_STR) for PSP slim
else
ifdef KERNEL_MODE
PSP_EBOOT_TITLE = $(PBPNAME_STR) $(VERSION_STR)
else
PSP_EBOOT_TITLE = $(PBPNAME_STR) $(VERSION_STR) for PSP
endif
endif


#------------------------------------------------------------------------------
# Object File Output Directtory
#------------------------------------------------------------------------------

OBJ = obj_cps1

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
	$(OBJ)/cpu/m68000 \
	$(OBJ)/cpu/z80 \
	$(OBJ)/cps1


#------------------------------------------------------------------------------
# Object Files (common)
#------------------------------------------------------------------------------

MAINOBJS += \
	$(OBJ)/cpu/m68000/m68000.o \
	$(OBJ)/cpu/m68000/c68k.o \
	$(OBJ)/cpu/z80/z80.o \
	$(OBJ)/cpu/z80/cz80.o \
	$(OBJ)/common/coin.o


#------------------------------------------------------------------------------
# Object Files (CPS1PSP)
#------------------------------------------------------------------------------

COREOBJS = \
	$(OBJ)/cps1/cps1.o \
	$(OBJ)/cps1/driver.o \
	$(OBJ)/cps1/memintrf.o \
	$(OBJ)/cps1/inptport.o \
	$(OBJ)/cps1/dipsw.o \
	$(OBJ)/cps1/timer.o \
	$(OBJ)/cps1/vidhrdw.o \
	$(OBJ)/cps1/sprite.o \
	$(OBJ)/cps1/eeprom.o \
	$(OBJ)/cps1/kabuki.o \
	$(OBJ)/sound/2151intf.o \
	$(OBJ)/sound/ym2151.o \
	$(OBJ)/sound/qsound.o

ICONOBJS = \
	$(OBJ)/$(OS)/icon/cps_s.o \
	$(OBJ)/$(OS)/icon/cps_l.o
