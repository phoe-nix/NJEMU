#------------------------------------------------------------------------------
#
#                             CPS2PSP Makefile
#
#------------------------------------------------------------------------------

PSP_EBOOT_ICON = data/cps2.png

ifdef PSP_SLIM
PSP_EBOOT_TITLE = $(PBPNAME_STR) $(VERSION_STR) for PSP slim
else
ifdef KERNEL_MODE
PSP_EBOOT_TITLE = $(PBPNAME_STR) $(VERSION_STR)
else
PSP_EBOOT_TITLE = $(PBPNAME_STR) $(VERSION_STR) for FW 3.xx
endif
endif

#------------------------------------------------------------------------------
# Object File Output Directtory
#------------------------------------------------------------------------------

OBJ = obj_cps2

#------------------------------------------------------------------------------
# File include path
#------------------------------------------------------------------------------

INCDIR += \
	src/cpu/m68000 \
	src/cpu/z80 \
	src/cps2


#------------------------------------------------------------------------------
# Object Directory
#------------------------------------------------------------------------------

OBJDIRS += \
	$(OBJ)/cpu/m68000 \
	$(OBJ)/cpu/z80 \
	$(OBJ)/cps2


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
# Object Files
#------------------------------------------------------------------------------

COREOBJS = \
	$(OBJ)/cps2/cps2.o \
	$(OBJ)/cps2/cps2crpt.o \
	$(OBJ)/cps2/driver.o \
	$(OBJ)/cps2/memintrf.o \
	$(OBJ)/cps2/inptport.o \
	$(OBJ)/cps2/timer.o \
	$(OBJ)/cps2/vidhrdw.o \
	$(OBJ)/cps2/sprite.o \
	$(OBJ)/cps2/eeprom.o \
	$(OBJ)/sound/qsound.o

ICONOBJS = \
	$(OBJ)/$(OS)/icon/cps_s.o \
	$(OBJ)/$(OS)/icon/cps_l.o
