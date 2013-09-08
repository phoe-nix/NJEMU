#------------------------------------------------------------------------------
#
#                              NCDZPSP Makefile
#
#------------------------------------------------------------------------------

PSP_EBOOT_ICON = data/ncdz.png

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

OBJ = obj_ncdz

#------------------------------------------------------------------------------
# File include path
#------------------------------------------------------------------------------

INCDIR += \
	src/cpu/m68000 \
	src/cpu/z80 \
	src/ncdz


#------------------------------------------------------------------------------
# Object Directory
#------------------------------------------------------------------------------

OBJDIRS += \
	$(OBJ)/cpu/m68000 \
	$(OBJ)/cpu/z80 \
	$(OBJ)/ncdz


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
	$(OBJ)/ncdz/ncdz.o \
	$(OBJ)/ncdz/cdda.o \
	$(OBJ)/ncdz/cdrom.o \
	$(OBJ)/ncdz/driver.o \
	$(OBJ)/ncdz/memintrf.o \
	$(OBJ)/ncdz/inptport.o \
	$(OBJ)/ncdz/timer.o \
	$(OBJ)/ncdz/vidhrdw.o \
	$(OBJ)/ncdz/sprite.o \
	$(OBJ)/sound/2610intf.o \
	$(OBJ)/sound/ym2610.o \

ICONOBJS = \
	$(OBJ)/$(OS)/icon/ncdz_s.o \
	$(OBJ)/$(OS)/icon/ncdz_l.o

OSOBJS += \
	$(OBJ)/$(OS)/mp3.o

LIBS += src/libmad/libmad.a
