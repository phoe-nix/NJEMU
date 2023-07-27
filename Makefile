#------------------------------------------------------------------------------
#
#            CPS1/CPS2/NEOGEO/NEOGEO CDZ Emulator for PSP Makefile
#
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Configuration
#------------------------------------------------------------------------------

#BUILD_CPS1PSP = 1
BUILD_CPS2PSP = 1
#BUILD_MVSPSP = 1
#BUILD_NCDZPSP = 1

PSP_SLIM = 1
#KERNEL_MODE = 1
COMMAND_LIST = 1
ADHOC = 1
SAVE_STATE = 1
UI_32BPP = 1
#RELEASE = 1

#------------------------------------------------------------------------------
# Version
#------------------------------------------------------------------------------

VERSION_MAJOR = 2
VERSION_MINOR = 3
VERSION_BUILD = 5


#------------------------------------------------------------------------------
# Defines
#------------------------------------------------------------------------------

OS = psp

ifdef PSP_SLIM
PSP_FW_VERSION = 371
KERNEL_MODE =
else
PSP_FW_VERSION = 150
endif

ifdef BUILD_CPS1PSP
BUILD_CPS2PSP=
BUILD_MVSPSP=
BUILD_NCDZPSP=
TARGET = CPS1PSP
endif

ifdef BUILD_CPS2PSP
BUILD_MVSPSP=
BUILD_NCDZPSP=
TARGET = CPS2PSP
endif

ifdef BUILD_MVSPSP
BUILD_NCDZPSP=
TARGET = MVSPSP
endif

ifdef BUILD_NCDZPSP
TARGET = NCDZPSP
ADHOC =
endif

PBPNAME_STR = $(TARGET)
ifeq ($(VERSION_BUILD), 0)
VERSION_STR = $(VERSION_MAJOR).$(VERSION_MINOR)
else
VERSION_STR = $(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_BUILD)
endif

ifdef BUILD_CPS1PSP
ifdef PSP_SLIM
EXTRA_TARGETS = maketree SLIM/CPS1/EBOOT.PBP delelf
else
EXTRA_TARGETS = maketree 3XX/CPS1/EBOOT.PBP delelf
endif
endif

ifdef BUILD_CPS2PSP
ifdef PSP_SLIM
EXTRA_TARGETS = maketree SLIM/CPS2/EBOOT.PBP delelf
else
EXTRA_TARGETS = maketree 3XX/CPS2/EBOOT.PBP delelf
endif
endif

ifdef BUILD_MVSPSP
ifdef PSP_SLIM
EXTRA_TARGETS = maketree SLIM/MVS/EBOOT.PBP delelf
else
EXTRA_TARGETS = maketree 3XX/MVS/EBOOT.PBP delelf
endif
endif

ifdef BUILD_NCDZPSP
ifdef PSP_SLIM
EXTRA_TARGETS = maketree SLIM/NCDZ/EBOOT.PBP delelf
else
EXTRA_TARGETS = maketree 3XX/NCDZ/EBOOT.PBP delelf
endif
endif

#------------------------------------------------------------------------------
# Utilities
#------------------------------------------------------------------------------

MD = -mkdir
RM = -rm

#------------------------------------------------------------------------------
# File include path
#------------------------------------------------------------------------------

INCDIR = \
	src \
	src/zip \
	src/zlib \
	src/libmad

ifdef PSP_SLIM
INCDIR += src/SDK/include
endif


#------------------------------------------------------------------------------
# Object Directory
#------------------------------------------------------------------------------

OBJDIRS = \
	$(OBJ) \
	$(OBJ)/cpu \
	$(OBJ)/common \
	$(OBJ)/sound \
	$(OBJ)/zip \
	$(OBJ)/zlib \
	$(OBJ)/libmad \
	$(OBJ)/$(OS) \
	$(OBJ)/$(OS)/font \
	$(OBJ)/$(OS)/icon


#------------------------------------------------------------------------------
# Object Files (common)
#------------------------------------------------------------------------------

MAINOBJS = \
	$(OBJ)/emumain.o \
	$(OBJ)/zip/zfile.o \
	$(OBJ)/zip/unzip.o \
	$(OBJ)/sound/sndintrf.o \
	$(OBJ)/common/cache.o \
	$(OBJ)/common/loadrom.o

ifdef ADHOC
MAINOBJS += $(OBJ)/common/adhoc.o
endif

ifdef COMMAND_LIST
MAINOBJS += $(OBJ)/common/cmdlist.o
endif

ifdef SAVE_STATE
MAINOBJS += $(OBJ)/common/state.o
endif

FONTOBJS = \
	$(OBJ)/$(OS)/font/graphic.o \
	$(OBJ)/$(OS)/font/ascii_14p.o \
	$(OBJ)/$(OS)/font/font_s.o \
	$(OBJ)/$(OS)/font/bshadow.o \
	$(OBJ)/$(OS)/font/command.o \
	$(OBJ)/$(OS)/font/ascii_14.o \
	$(OBJ)/$(OS)/font/latin1_14.o \
	$(OBJ)/$(OS)/font/gbk_s14.o \
	$(OBJ)/$(OS)/font/gbk_tbl.o

OSOBJS = \
	$(OBJ)/$(OS)/$(OS).o \
	$(OBJ)/$(OS)/config.o \
	$(OBJ)/$(OS)/filer.o \
	$(OBJ)/$(OS)/input.o \
	$(OBJ)/$(OS)/ticker.o \
	$(OBJ)/$(OS)/ui.o \
	$(OBJ)/$(OS)/ui_draw.o \
	$(OBJ)/$(OS)/ui_menu.o \
	$(OBJ)/$(OS)/ui_text.o \
	$(OBJ)/$(OS)/video.o \
	$(OBJ)/$(OS)/sound.o \
	$(OBJ)/$(OS)/png.o \

ifdef ADHOC
OSOBJS += $(OBJ)/$(OS)/adhoc.o
endif

OSOBJS += $(OBJ)/$(OS)/SystemButtons.o

ifdef UI_32BPP
OSOBJS += $(OBJ)/$(OS)/wallpaper.o
endif

ZLIB = \
	$(OBJ)/zlib/adler32.o \
	$(OBJ)/zlib/compress.o \
	$(OBJ)/zlib/uncompr.o \
	$(OBJ)/zlib/crc32.o \
	$(OBJ)/zlib/deflate.o \
	$(OBJ)/zlib/inflate.o \
	$(OBJ)/zlib/inftrees.o \
	$(OBJ)/zlib/inffast.o \
	$(OBJ)/zlib/trees.o \
	$(OBJ)/zlib/zutil.o

#	$(OBJ)/zlib/gzclose.o \
#	$(OBJ)/zlib/gzlib.o \
#	$(OBJ)/zlib/gzread.o \
#	$(OBJ)/zlib/gzwrite.o \
#	$(OBJ)/zlib/infback.o \
#------------------------------------------------------------------------------
# Include makefiles
#------------------------------------------------------------------------------

include src/makefiles/$(TARGET).mak


#------------------------------------------------------------------------------
# Compiler Flags
#------------------------------------------------------------------------------

CFLAGS = \
	-O2 \
	-fomit-frame-pointer \
	-fstrict-aliasing \
	-falign-functions=32 \
	-falign-loops \
	-falign-labels \
	-falign-jumps \
	-Wall \
	-Wundef \
	-Wpointer-arith  \
	-Wbad-function-cast \
	-Wwrite-strings \
	-Wmissing-prototypes \
	-Wsign-compare \
	-Werror \
	-DZLIB_CONST


#------------------------------------------------------------------------------
# Compiler Defines
#------------------------------------------------------------------------------

CDEFS = -DINLINE='static __inline' \
	-Dinline=__inline \
	-D__inline__=__inline \
	-DBUILD_$(TARGET)=1 \
	-DPBPNAME_STR='"$(PBPNAME_STR)"' \
	-DVERSION_STR='"$(VERSION_STR)"' \
	-DVERSION_MAJOR=$(VERSION_MAJOR) \
	-DVERSION_MINOR=$(VERSION_MINOR) \
	-DVERSION_BUILD=$(VERSION_BUILD) \
	-DPSP

ifdef PSP_SLIM
CDEFS += -DPSP_SLIM=1
endif

ifdef KERNEL_MODE
CDEFS += -DKERNEL_MODE=1
endif

ifdef ADHOC
CDEFS += -DADHOC=1
SAVE_STATE = 1
endif

ifdef SAVE_STATE
CDEFS += -DSAVE_STATE=1
endif

ifdef COMMAND_LIST
CFLAGS += -DCOMMAND_LIST=1
endif

ifdef UI_32BPP
CFLAGS += -DPSP_VIDEO_32BPP=1
else
CFLAGS += -DPSP_VIDEO_32BPP=0
endif

ifdef RELEASE
CDEFS += -DRELEASE=1
else
CDEFS += -DRELEASE=0
endif

#------------------------------------------------------------------------------
# Linker Flags
#------------------------------------------------------------------------------

ifdef PSP_SLIM
LIBDIR = src/SDK/lib
else
LIBDIR =
endif
LDFLAGS =


#------------------------------------------------------------------------------
# Library
#------------------------------------------------------------------------------

USE_PSPSDK_LIBC = 1

LIBS = -lpspaudio -lpspgu -lpsppower

ifdef PSP_SLIM
LIBS += -lpspkubridge
endif

ifdef ADHOC
LIBS += -lpspwlan -lpspnet_adhoc -lpspnet_adhocctl -lpspnet_adhocmatching
endif

ifdef BUILD_NCDZPSP
LIBS += src/libmad/libmad.a
endif

#------------------------------------------------------------------------------
# Rules to make libraries
#------------------------------------------------------------------------------

OBJS = $(MAINOBJS) $(COREOBJS) $(OSOBJS) $(FONTOBJS) $(ICONOBJS) $(ZLIB)

include src/makefiles/build.mak


#------------------------------------------------------------------------------
# Rules to manage files
#------------------------------------------------------------------------------

$(OBJ)/%.o: src/%.c
	@echo Compiling $<...
	@$(CC) $(CDEFS) $(CFLAGS) -c $< -o$@

ifdef KERNEL_MODE
$(OBJ)/psp/adhoc.o: src/psp/adhoc.c
	@echo Compiling [-G0] $<...
	@$(CC) -G0 $(CDEFS) $(CFLAGS) -c $< -o$@
endif

$(OBJ)/%.o: src/%.S
	@echo Assembling $<...
	@$(CC) $(CDEFS) $(CFLAGS) -c $< -o$@

$(OBJ)/%.o: src/%.s
	@echo Assembling $<...
	@$(AS) $(ASDEFS) $(ASFLAGS) -c $< -o$@

$(OBJ)/%.a:
	@echo Archiving $@...
	@$(AR) -r $@ $^

delelf:
	@$(RM) -f $(PSP_EBOOT_SFO)
	@$(RM) -f $(TARGET).elf

maketree:
	@$(MD) 3XX
	@$(MD) 3XX/CPS1
	@$(MD) 3XX/CPS2
	@$(MD) 3XX/MVS
	@$(MD) 3XX/NCDZ
	@$(MD) SLIM
	@$(MD) SLIM/CPS1
	@$(MD) SLIM/CPS2
	@$(MD) SLIM/MVS
	@$(MD) SLIM/NCDZ
	@$(MD) -p $(subst //,\,$(sort $(OBJDIRS)))
