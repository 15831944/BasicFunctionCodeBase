#*****************************************************
#
# 北京裕兴软件有限公司版权所有
#
#*****************************************************

CROSS_COMPILE := arm-vfp_uclibc-linux-gnu-

############################################################################
#	every one should has own configs
#	start
#
#hisi complier path
TOOLCHAIN_DIR=$(shell dirname $(shell dirname $(shell which arm-vfp_uclibc-linux-gnu-gcc)))
ROOTFS_PATH = ../../

TARGET := usb.elf

#
# This is the define of complier in here.
# the varible  must be defined outside.
#
# Define make variables
AS      = $(CROSS_COMPILE)as
LD	= $(CROSS_COMPILE)gcc
CC      = $(CROSS_COMPILE)gcc

# NOTE: We must use c++ and not g++ to avoid GPL contamination
CXX     = $(CROSS_COMPILE)c++
AR      = $(CROSS_COMPILE)ar
NM      = $(CROSS_COMPILE)nm
STRIP   = $(CROSS_COMPILE)strip
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
RANLIB  = $(CROSS_COMPILE)ranlib
MKDIR   = mkdir -p
MV      = mv
RM		= rm -rf
CP		= cp

# system macro declare
DEFINES := -DLINUX_OS

INCLUDES  = -I$(TOOLCHAIN_DIR)/include

CFLAGS		:= -g -Wall -O2
CXXFLAGS	:= 
CUSERFLAGS  := 

#opti
CFLAGS		+=  -march=armv5te -mtune=arm9tdmi


LDFLAGS := 
LDFLAGS += -L$(TOOLCHAIN_DIR)/lib/ 
ARFLAGS = -curs

SRCDIR = .
OBJDIR = ./objs

OBJS = mountusb.obj

LIBS	:= 
LIBS    += -lpthread


.PHONY: test clean build_time images

#all: $(TARGET)


$(TARGET): $(OBJS)
	@echo	Linking $@
	$(LD) $(LDFLAGS) -Wl,--start-group $(OBJS) $(LIBS) -lgcc -lm -Wl,--end-group -o $@ 
	chmod 777 $(TARGET)
	$(CP) $@ ../../rootfs/home/yx5532/bin/
	chmod 755 -R ../../rootfs/home/yx5532/bin/

build_time:
	touch time_build.c

#
# 编译 C 文件
#
$(OBJS):mountusb.c
	@echo	Compiling $< ......
	$(CC) $(CFLAGS) $(DEFINES) $(CUSERFLAGS) $(INCLUDES) -o $@ -c  $<
	
#
# 清除目标文件
#
clean:
	@echo deleting $(CUSTOMER_OBJS)
	$(RM)	-rf usb.elf
	$(RM)	-rf *.o *.obj

