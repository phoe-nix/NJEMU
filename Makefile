#------------------------------------------------------------------------------
#
#            CPS1/CPS2/NEOGEO/NEOGEO CDZ Emulator for PSP Makefile
#
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Configuration
#------------------------------------------------------------------------------

#BUILD_CPS1 = 1
BUILD_CPS2 = 1
#BUILD_MVS = 1
#BUILD_NCDZ = 1

LARGE_MEMORY = 1
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
VERSION_MINOR = 4
VERSION_BUILD = 0


#------------------------------------------------------------------------------
# Defines
#------------------------------------------------------------------------------

OS = psp
SYSTEM_NAME = PSP

ifdef BUILD_CPS1
BUILD_CPS2=
BUILD_MVS=
BUILD_NCDZ=
TARGET = CPS1
endif

ifdef BUILD_CPS2
BUILD_MVS=
BUILD_NCDZ=
TARGET = CPS2
endif

ifdef BUILD_MVS
BUILD_NCDZ=
TARGET = MVS
endif

ifdef BUILD_NCDZ
TARGET = NCDZ
ADHOC =
endif

PBPNAME_STR = $(TARGET)
ifeq ($(VERSION_BUILD), 0)
VERSION_STR = $(VERSION_MAJOR).$(VERSION_MINOR)
else
VERSION_STR = $(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_BUILD)
endif

ifdef BUILD_CPS1
ifdef LARGE_MEMORY
EXTRA_TARGETS = maketree SLIM/CPS1/EBOOT.PBP
else
EXTRA_TARGETS = maketree 3XX/CPS1/EBOOT.PBP
endif
endif

ifdef BUILD_CPS2
ifdef LARGE_MEMORY
EXTRA_TARGETS = maketree SLIM/CPS2/EBOOT.PBP
else
EXTRA_TARGETS = maketree 3XX/CPS2/EBOOT.PBP
endif
endif

ifdef BUILD_MVS
ifdef LARGE_MEMORY
EXTRA_TARGETS = maketree SLIM/MVS/EBOOT.PBP
else
EXTRA_TARGETS = maketree 3XX/MVS/EBOOT.PBP
endif
endif

ifdef BUILD_NCDZ
ifdef LARGE_MEMORY
EXTRA_TARGETS = maketree SLIM/NCDZ/EBOOT.PBP
else
EXTRA_TARGETS = maketree 3XX/NCDZ/EBOOT.PBP
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


#------------------------------------------------------------------------------
# Object Files (common)
#------------------------------------------------------------------------------

MAINOBJS = \
	emumain.o \
	zip/zfile.o \
	zip/unzip.o \
	sound/sndintrf.o \
	common/cache.o \
	common/loadrom.o

ifdef ADHOC
MAINOBJS += common/adhoc.o
endif

ifdef COMMAND_LIST
MAINOBJS += common/cmdlist.o
endif

ifdef SAVE_STATE
MAINOBJS += common/state.o
endif

FONTOBJS = \
	$(OS)/font/graphic.o \
	$(OS)/font/ascii_14p.o \
	$(OS)/font/font_s.o \
	$(OS)/font/bshadow.o \
	$(OS)/font/command.o \
	$(OS)/font/ascii_14.o \
	$(OS)/font/latin1_14.o \
	$(OS)/font/gbk_s14.o \
	$(OS)/font/gbk_tbl.o

OSOBJS = \
	$(OS)/$(OS).o \
	$(OS)/cpuclock.o \
	$(OS)/config.o \
	$(OS)/filer.o \
	$(OS)/input.o \
	$(OS)/ticker.o \
	$(OS)/ui.o \
	$(OS)/ui_draw.o \
	$(OS)/ui_menu.o \
	$(OS)/ui_text.o \
	$(OS)/video.o \
	$(OS)/sound.o \
	$(OS)/png.o \

ifdef ADHOC
OSOBJS += $(OS)/adhoc.o
endif

OSOBJS += $(OS)/SystemButtons.o

ifdef UI_32BPP
OSOBJS += $(OS)/wallpaper.o
endif

#------------------------------------------------------------------------------
# Include makefiles
#------------------------------------------------------------------------------

include src/makefiles/$(TARGET)$(SYSTEM_NAME).mak


#------------------------------------------------------------------------------
# Compiler Flags
#------------------------------------------------------------------------------

CFLAGS = \
	-O3 \
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
	-Wsign-compare


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

ifdef LARGE_MEMORY
CDEFS += -DLARGE_MEMORY=1
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
CFLAGS += -DVIDEO_32BPP=1
else
CFLAGS += -DVIDEO_32BPP=0
endif

ifdef RELEASE
CDEFS += -DRELEASE=1
else
CDEFS += -DRELEASE=0
endif

CFLAGS += $(CDEFS)

#------------------------------------------------------------------------------
# Linker Flags
#------------------------------------------------------------------------------

LIBDIR =
LDFLAGS = -L$(shell psp-config --psp-prefix)


#------------------------------------------------------------------------------
# Library
#------------------------------------------------------------------------------

LIBS = -lpspaudio -lpspgu -lpsppower -lz

ifdef LARGE_MEMORY
LIBS += -lpspkubridge
endif

ifdef ADHOC
LIBS += -lpspwlan -lpspnet_adhoc -lpspnet_adhocctl -lpspnet_adhocmatching
endif

ifdef BUILD_NCDZ
LIBS += -lmad
endif

#------------------------------------------------------------------------------
# Rules to make libraries
#------------------------------------------------------------------------------

ALLOBJS = $(MAINOBJS) $(COREOBJS) $(OSOBJS) $(FONTOBJS) $(ICONOBJS)
OBJS = $(ALLOBJS:%=src/%)


ifdef BUILD_CPS1
ifdef LARGE_MEMORY
PSP_EBOOT = SLIM/CPS1/EBOOT.PBP
else
PSP_EBOOT = 3XX/CPS1/EBOOT.PBP
endif
endif

ifdef BUILD_CPS2
ifdef LARGE_MEMORY
PSP_EBOOT = SLIM/CPS2/EBOOT.PBP
else
PSP_EBOOT = 3XX/CPS2/EBOOT.PBP
endif
endif

ifdef BUILD_MVS
ifdef LARGE_MEMORY
PSP_EBOOT = SLIM/MVS/EBOOT.PBP
else
PSP_EBOOT = 3XX/MVS/EBOOT.PBP
endif
endif

ifdef BUILD_NCDZ
ifdef LARGE_MEMORY
PSP_EBOOT = SLIM/NCDZ/EBOOT.PBP
else
PSP_EBOOT = 3XX/NCDZ/EBOOT.PBP
endif
endif

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

#------------------------------------------------------------------------------
# Rules to manage files
#------------------------------------------------------------------------------

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
