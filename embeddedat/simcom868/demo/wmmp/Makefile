##########################################################################
# File:             Mailefile 
# Version:          0.1 
# Author:           Maobin
# DATE              NAME                        DESCRIPTION
# 2013-01-10        Maobin                      Initial Version 0.1
##########################################################################
# started
include $(PROJ_HOME)/build/option.mak
OBJDIR=./
MODULE_NAME = $(PARAM)
LIB_RESULE=$(MODULE_NAME).a

#Two mothods#
#First mothod#
#S1 := $(wildcard *.c)
#SOURCE:=$(S1)

#Second mothod#
SOURCE := main.c
SOURCE += src/simcom_wmmp_utility.c \
          src/simcom_wmmp_hdlr_ex.c \
          src/simcom_wmmp_nvram.c \
          src/simcom_wmmp_sms.c

SOURCE += wmmp30/src/wmmp_lib.c \
		  wmmp30/src/wmmp_encode.c \
		  wmmp30/src/wmmp_tlv.c \
		  wmmp30/src/wmmp_3des.c \
		  wmmp30/src/wmmp_crc.c \
		  wmmp30/src/wmmp_decode.c \
		  wmmp30/src/wmmp_md5.c

LOCAL_INCLUDE := -I./ \
				 -I inc \
				 -I wmmp30/inc

#编译为.a库文件
include $(PROJ_HOME)/build/app_build.mak

#示例中只有一个文件，编译成main.o
#OBJECT := main.o

#all:$(OBJECT)

#$(OBJECT):$(SOURCE)
#	$(CC) -c $(CFLAGS) $(LOCAL_INCLUDE) -o $(LIB_DIR)/$@ $< 
#
#.PHONY:
#   clean:
#	rm -fr $(OBJECT) $(LIB_DIR)/$(LIB_RESULE)
