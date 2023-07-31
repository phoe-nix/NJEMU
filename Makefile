#------------------------------------------------------------------------------
#
#            CPS1/CPS2/NEOGEO/NEOGEO CDZ Emulator for PSP Makefile
#
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Configuration
#------------------------------------------------------------------------------

BUILD_CPS1 = 0
BUILD_CPS2 = 1
BUILD_MVS = 0
BUILD_NCDZ = 0

LARGE_MEMORY = 1
KERNEL_MODE = 0
COMMAND_LIST = 1
ADHOC = 1
SAVE_STATE = 1
UI_32BPP = 1
RELEASE = 0

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

ifeq ($(BUILD_CPS1), 1)
TARGET = CPS1
PSP_EBOOT_ICON = data/cps1.png
endif

ifeq ($(BUILD_CPS2), 1) 
TARGET = CPS2
PSP_EBOOT_ICON = data/cps2.png
endif

ifeq ($(BUILD_MVS), 1)
TARGET = MVS
PSP_EBOOT_ICON = data/mvs.png
endif

ifeq ($(BUILD_NCDZ), 1)
TARGET = NCDZ
ADHOC = 0
PSP_EBOOT_ICON = data/ncdz.png
endif

ifeq ($(VERSION_BUILD), 0)
VERSION_STR = $(VERSION_MAJOR).$(VERSION_MINOR)
else
VERSION_STR = $(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_BUILD)
endif

EXTRA_TARGETS = EBOOT.PBP
ifeq ($(LARGE_MEMORY), 1)
PSP_EBOOT_TITLE = $(TARGET) $(VERSION_STR) for PSP Slim
else
PSP_EBOOT_TITLE = $(TARGET) $(VERSION_STR) for PSP
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
	common/loadrom.o \
	common/cpuclock.o \
	common/thread_driver.o \
	common/audio_driver.o \
	common/power_driver.o \
	common/ticker_driver.o \
	common/sound.o \

ifeq ($(ADHOC), 1)
MAINOBJS += common/adhoc.o
endif

ifeq ($(COMMAND_LIST), 1)
MAINOBJS += common/cmdlist.o
endif

ifeq ($(SAVE_STATE), 1)
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
	$(OS)/$(OS)_thread.o \
	$(OS)/$(OS)_audio.o \
	$(OS)/$(OS)_power.o \
	$(OS)/$(OS)_ticker.o \
	$(OS)/config.o \
	$(OS)/filer.o \
	$(OS)/input.o \
	$(OS)/ui.o \
	$(OS)/ui_draw.o \
	$(OS)/ui_menu.o \
	$(OS)/ui_text.o \
	$(OS)/video.o \
	$(OS)/png.o \

ifeq ($(ADHOC), 1)
OSOBJS += $(OS)/adhoc.o
endif

OSOBJS += $(OS)/SystemButtons.o

ifeq ($(UI_32BPP), 1)
OSOBJS += $(OS)/wallpaper.o
endif

#------------------------------------------------------------------------------
# Include makefiles
#------------------------------------------------------------------------------

include src/makefiles/$(TARGET).mak


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
	-DTARGET_STR='"$(TARGET)"' \
	-DVERSION_STR='"$(VERSION_STR)"' \
	-DVERSION_MAJOR=$(VERSION_MAJOR) \
	-DVERSION_MINOR=$(VERSION_MINOR) \
	-DVERSION_BUILD=$(VERSION_BUILD) \
	-DPSP

ifeq ($(LARGE_MEMORY), 1)
CDEFS += -DLARGE_MEMORY=1
endif

ifeq ($(KERNEL_MODE), 1)
CDEFS += -DKERNEL_MODE=1
endif

ifeq ($(ADHOC), 1)
CDEFS += -DADHOC=1
SAVE_STATE = 1
endif

ifeq ($(SAVE_STATE), 1)
CDEFS += -DSAVE_STATE=1
endif

ifeq ($(COMMAND_LIST), 1)
CFLAGS += -DCOMMAND_LIST=1
endif

ifeq ($(UI_32BPP), 1)
CFLAGS += -DVIDEO_32BPP=1
else
CFLAGS += -DVIDEO_32BPP=0
endif

ifeq ($(RELEASE), 1)
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

ifeq ($(LARGE_MEMORY), 1)
LIBS += -lpspkubridge
endif

ifeq ($(ADHOC), 1)
LIBS += -lpspwlan -lpspnet_adhoc -lpspnet_adhocctl -lpspnet_adhocmatching
endif

ifeq ($(BUILD_NCDZ), 1)
LIBS += -lmad
endif

#------------------------------------------------------------------------------
# Rules to make libraries
#------------------------------------------------------------------------------

ALLOBJS = $(MAINOBJS) $(COREOBJS) $(OSOBJS) $(FONTOBJS) $(ICONOBJS)
OBJS = $(ALLOBJS:%=src/%)

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
