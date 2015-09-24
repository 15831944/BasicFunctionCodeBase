#!/bin/sh

## readme

# Define make variables
AS      = $(CROSS_COMPILE)as
LD      = $(CROSS_COMPILE)ld
CC      = $(CROSS_COMPILE)gcc
# NOTE: We must use c++ and not g++ to avoid GPL contamination
CXX     = $(CROSS_COMPILE)g++
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



INCLUDES += -I.
INCLUDES +=	-I../include -I../include/log -I../include/utils 
INCLUDES += -idirafter dir  
    #-idirafter dir  在-I的目录里面查找失败之后，再在这个目录里面查找头文件，这样的参数为设置头文件查找的优先级问题比较有帮助。
    #-iprefix prefix
    #-iwithprefix dir   这两个参数一起用，在-I目录寻找失败的时候，到prefix的dir下查找头文件。
    
CFLAGS += -Wall -g -O2 
CFLAGS += -fPIC -fno-rtti -fno-exceptions
CXXFLAGS += -fno-strict-prototype

LDFLAGS += -lm 

ARFLAGS = -curs



SRCS = 	./base64/Base64.c \
		./ini-parser/dictionary.c \
        ./ini-parser/iniparser.c \
		./json/cJSON.c \






SRCDIR = .
OBJDIR = ./objs

OBJS = 
OBJS += $(OBJDIR)/base64/base64.obj

OBJS += $(OBJDIR)/ini-parser/dictionary.obj
OBJS += $(OBJDIR)/ini-parser/iniparser.obj

OBJS += $(OBJDIR)/json/cJSON.obj



.PHONY: test clean build_time

all: builddir

builddir:
	$(MKDIR) ./objs
	$(MKDIR) ./objs/base64
	$(MKDIR) ./objs/ini-parser
	$(MKDIR) ./objs/json


#
# 编译 C 文件
#
$(OBJDIR)/%.obj:$(SRCDIR)/%.c
	$(CC) $(CFLAGS) $(DEFINES) $(CUSERFLAGS) $(INCLUDES) -o $@ -c  $<

#
# 清除目标文件
#
clean:
	@echo deleting $(OBJDIR)
	$(RM)	-rf $(OBJDIR)




