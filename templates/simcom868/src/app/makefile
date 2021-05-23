##########################################################################
# File:             Mailefile 
# Version:          0.1 
# Author:           Maobin
# DATE              NAME                        DESCRIPTION
# 2011-04-10        Maobin                      Initial Version 0.1
##########################################################################
# started
include $(PROJ_HOME)/build/option.mak
OBJDIR=./
MODULE_NAME = $(PARAM)
LIB_RESULE=$(MODULE_NAME).a

#Two mothods#
#First mothod#
S1 := $(wildcard *.c)
SOURCE:=$(S1)

#Second mothod#
#SOURCE := app_demo_other.c

#There is only one source file in the example, so compile the source file as main.o
#MAIN_SRC := main.c  
#MAIN_OBJ := main.o 

#Header file directory settings
LOCAL_INCLUDE := -I./

#Compile and generate .a files
include $(PROJ_HOME)/build/app_build.mak

