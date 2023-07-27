#------------------------------------------------------------------------------
#
#                              NCDZPSP Makefile
#
#------------------------------------------------------------------------------

PSP_EBOOT_ICON = data/ncdz.png

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
	cpu/m68000 \
	cpu/z80 \
	ncdz


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
	ncdz/ncdz.o \
	ncdz/cdda.o \
	ncdz/cdrom.o \
	ncdz/driver.o \
	ncdz/memintrf.o \
	ncdz/inptport.o \
	ncdz/timer.o \
	ncdz/vidhrdw.o \
	ncdz/sprite.o \
	sound/2610intf.o \
	sound/ym2610.o \

ICONOBJS = \
	$(OS)/icon/ncdz_s.o \
	$(OS)/icon/ncdz_l.o

OSOBJS += \
	$(OS)/mp3.o
