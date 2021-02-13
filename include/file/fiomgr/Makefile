TARGET = fiomgr

INCDIR  = ../../../include
LIBPATH = ../../../lib

OBJS = $(patsubst %.c,%.o,$(wildcard *.c))

PSPSDK = $(shell psp-config --pspsdk-path)
include ../../build.opt ../../build.mak
