#------------------------------------------------------------------------------
#
#                             MVSPSP Makefile
#
#------------------------------------------------------------------------------

PSP_EBOOT_ICON = data/mvs.png

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

OBJ = obj_mvs

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
	$(OBJ)/cpu/m68000 \
	$(OBJ)/cpu/z80 \
	$(OBJ)/mvs


#------------------------------------------------------------------------------
# Object Files (common)
#------------------------------------------------------------------------------

MAINOBJS += \
	$(OBJ)/cpu/m68000/m68000.o \
	$(OBJ)/cpu/m68000/c68k.o \
	$(OBJ)/cpu/z80/z80.o \
	$(OBJ)/cpu/z80/cz80.o


#------------------------------------------------------------------------------
# Object Files
#------------------------------------------------------------------------------

COREOBJS = \
	$(OBJ)/mvs/mvs.o \
	$(OBJ)/mvs/driver.o \
	$(OBJ)/mvs/memintrf.o \
	$(OBJ)/mvs/inptport.o \
	$(OBJ)/mvs/dipsw.o \
	$(OBJ)/mvs/timer.o \
	$(OBJ)/mvs/vidhrdw.o \
	$(OBJ)/mvs/sprite.o \
	$(OBJ)/mvs/pd4990a.o \
	$(OBJ)/mvs/neocrypt.o \
	$(OBJ)/mvs/biosmenu.o \
	$(OBJ)/sound/2610intf.o \
	$(OBJ)/sound/ym2610.o

ICONOBJS = \
	$(OBJ)/$(OS)/icon/mvs_s.o \
	$(OBJ)/$(OS)/icon/mvs_l.o
