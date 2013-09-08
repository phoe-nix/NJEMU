# PSP Software Development Kit - http://www.pspdev.org
# -----------------------------------------------------------------------
# Licensed under the BSD license, see LICENSE in PSPSDK root for details.
#
# build.mak - Base makefile for projects using PSPSDK.
#
# Copyright (c) 2005 Marcus R. Brown <mrbrown@ocgnet.org>
# Copyright (c) 2005 James Forshaw <tyranid@gmail.com>
# Copyright (c) 2005 John Kelley <ps2dev@kelley.ca>
#
# $Id: build.mak 2214 2007-04-12 20:04:48Z jim $

# Note: The PSPSDK make variable must be defined before this file is included.
ifeq ($(PSPSDK),)
PSPSDK   = $(shell psp-config --pspsdk-path)
endif

CC       = psp-gcc
CXX      = psp-g++
AS       = psp-gcc
LD       = psp-gcc
AR       = psp-ar
RANLIB   = psp-ranlib
STRIP    = psp-strip
MKSFO    = mksfo
PACK_PBP = pack-pbp
FIXUP    = psp-fixup-imports

# Add in PSPSDK includes and libraries.
INCDIR   := $(INCDIR) . $(PSPSDK)/include
LIBDIR   := $(LIBDIR) . $(PSPSDK)/lib

CFLAGS   := $(addprefix -I,$(INCDIR)) $(CFLAGS)
CXXFLAGS := $(CFLAGS) $(CXXFLAGS)
ASFLAGS  := $(CFLAGS) $(ASFLAGS)
LDFLAGS  := $(addprefix -L,$(LIBDIR)) $(LDFLAGS)

ifeq ($(PSP_FW_VERSION),)
PSP_FW_VERSION=150
endif

CFLAGS += -D_PSP_FW_VERSION=$(PSP_FW_VERSION)

# Library selection.  By default we link with Newlib's libc.  Allow the
# user to link with PSPSDK's libc if USE_PSPSDK_LIBC is set to 1.

ifeq ($(USE_KERNEL_LIBC),1)
# Use the PSP's kernel libc
PSPSDK_LIBC_LIB = 
CFLAGS := -I$(PSPSDK)/include/libc $(CFLAGS)
else
ifeq ($(USE_PSPSDK_LIBC),1)
# Use the pspsdk libc
PSPSDK_LIBC_LIB = -lpsplibc
CFLAGS := -I$(PSPSDK)/include/libc $(CFLAGS)
else
# Use newlib (urgh)
PSPSDK_LIBC_LIB = -lc
endif
endif


# Link with following default libraries.  Other libraries should be specified in the $(LIBS) variable.
# TODO: This library list needs to be generated at configure time.
#
ifeq ($(USE_KERNEL_LIBS),1)
PSPSDK_LIBS = -lpspdebug -lpspdisplay_driver -lpspctrl_driver -lpspsdk
LIBS     := $(LIBS) $(PSPSDK_LIBS) $(PSPSDK_LIBC_LIB) -lpspkernel
else
ifeq ($(USE_USER_LIBS),1)
PSPSDK_LIBS = -lpspdebug -lpspdisplay -lpspge -lpspctrl -lpspsdk
LIBS     := $(LIBS) $(PSPSDK_LIBS) $(PSPSDK_LIBC_LIB) -lpspnet \
			-lpspnet_inet -lpspnet_apctl -lpspnet_resolver -lpsputility \
			-lpspuser
else
PSPSDK_LIBS = -lpspdebug -lpspdisplay -lpspge -lpspctrl -lpspsdk
LIBS     := $(LIBS) $(PSPSDK_LIBS) $(PSPSDK_LIBC_LIB) -lpspnet \
			-lpspnet_inet -lpspnet_apctl -lpspnet_resolver -lpsputility \
			-lpspuser -lpspkernel
endif
endif

# Define the overridable parameters for EBOOT.PBP
ifndef PSP_EBOOT_TITLE
PSP_EBOOT_TITLE = $(TARGET)
endif

ifndef PSP_EBOOT_SFO
PSP_EBOOT_SFO = PARAM.SFO
endif

ifndef PSP_EBOOT_ICON
PSP_EBOOT_ICON = NULL
endif

ifndef PSP_EBOOT_ICON1
PSP_EBOOT_ICON1 = NULL
endif

ifndef PSP_EBOOT_UNKPNG
PSP_EBOOT_UNKPNG = NULL
endif

ifndef PSP_EBOOT_PIC1
PSP_EBOOT_PIC1 = NULL
endif

ifndef PSP_EBOOT_SND0
PSP_EBOOT_SND0 = NULL
endif

ifndef PSP_EBOOT_PSAR
PSP_EBOOT_PSAR = NULL
endif

ifndef PSP_EBOOT
PSP_EBOOT = EBOOT.PBP
endif

FINAL_TARGET = $(TARGET).elf

all: $(EXTRA_TARGETS) $(FINAL_TARGET)

kxploit: maketree $(TARGET).elf $(PSP_EBOOT_SFO)
	$(MD) -p $(TARGET)
	$(STRIP) $(TARGET).elf -o $(TARGET)/$(PSP_EBOOT)
	$(MD) -p $(TARGET)%
	$(PACK_PBP) "$(TARGET)%/$(PSP_EBOOT)" $(PSP_EBOOT_SFO) $(PSP_EBOOT_ICON)  \
		$(PSP_EBOOT_ICON1) $(PSP_EBOOT_UNKPNG) $(PSP_EBOOT_PIC1)  \
		$(PSP_EBOOT_SND0) NULL $(PSP_EBOOT_PSAR)
	$(RM) -f $(PSP_EBOOT_SFO)
	$(RM) -f $(TARGET).elf

SCEkxploit: maketree $(TARGET).elf $(PSP_EBOOT_SFO)
	$(MD) -p __SCE__$(TARGET)
	$(STRIP) $(TARGET).elf -o __SCE__$(TARGET)/$(PSP_EBOOT)
	$(MD) -p %__SCE__$(TARGET)
	$(PACK_PBP) "%__SCE__$(TARGET)/$(PSP_EBOOT)" $(PSP_EBOOT_SFO) $(PSP_EBOOT_ICON)  \
		$(PSP_EBOOT_ICON1) $(PSP_EBOOT_UNKPNG) $(PSP_EBOOT_PIC1)  \
		$(PSP_EBOOT_SND0) NULL $(PSP_EBOOT_PSAR)
	$(RM) -f $(PSP_EBOOT_SFO)
	$(RM) -f $(TARGET).elf

$(TARGET).elf: $(OBJS) $(EXPORT_OBJ)
	$(LINK.c) $^ $(LIBS) -o $@
	$(FIXUP) $@

$(TARGET_LIB): $(OBJS)
	$(AR) cru $@ $(OBJS)
	$(RANLIB) $@

$(PSP_EBOOT_SFO): 
	$(MKSFO) '$(PSP_EBOOT_TITLE)' $@

$(PSP_EBOOT): $(FINAL_TARGET) $(PSP_EBOOT_SFO)
	$(STRIP) $(FINAL_TARGET) -o strip_$(FINAL_TARGET)
	$(PACK_PBP) EBOOT.PBP $(PSP_EBOOT_SFO) $(PSP_EBOOT_ICON)  \
		$(PSP_EBOOT_ICON1) $(PSP_EBOOT_UNKPNG) $(PSP_EBOOT_PIC1)  \
		$(PSP_EBOOT_SND0)  strip_$(FINAL_TARGET) $(PSP_EBOOT_PSAR)
	$(RM) -f strip_$(FINAL_TARGET)

%.prx: %.elf
	psp-prxgen $< $@

%.c: %.exp
	psp-build-exports -b $< > $@

clean: 
	$(RM) -f $(FINAL_TARGET) $(EXTRA_CLEAN) $(OBJS) $(PSP_EBOOT_SFO) $(PSP_EBOOT) $(EXTRA_TARGETS)

rebuild: clean all
