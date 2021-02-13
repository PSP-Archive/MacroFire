TARGET = dmem_kernel

CFLAGS  = -DHEAP_KERNEL
INCDIR  = ../../../include
LIBPATH = ../../../lib

OBJS = $(notdir $(patsubst %.c,%.o,$(wildcard ../dmem/*.c)))

%.o: ../dmem/%.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $(@F)

PSPSDK = $(shell psp-config --pspsdk-path)
include ../../build.opt ../../build.mak
