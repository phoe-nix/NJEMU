TARGET = SystemButtons
OBJS = main.o sceCtrl_driver_371.o
PSP_FW_VERSION = 150

INCDIR =
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

BUILD_PRX = 1
PRX_EXPORTS = $(TARGET).exp

USE_KERNEL_LIBC=1
USE_KERNEL_LIBS=1

LIBDIR =
LDFLAGS = -mno-crt0 -nostartfiles
LIBS = -lpspsdk

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

all:
	psp-build-exports -s $(PRX_EXPORTS)
	@cp $(TARGET).prx ../
	@cp $(TARGET).S ../src/psp/
	@cp $(TARGET).h ../src/psp/
	@rm -f $(TARGET).prx
	@rm -f $(TARGET).S
	@rm -f *.o
	@rm -f *.elf
