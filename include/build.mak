CC  = psp-gcc
CXX = psp-g++
AS  = psp-gcc
LD  = psp-gcc
AR  = psp-ar

INCDIR := $(INCDIR) . $(PSPSDK)/include $(PSPSDK)/include/libc

CFLAGS   := $(addprefix -I,$(INCDIR)) $(CFLAGS)
CXXFLAGS := $(CFLAGS) $(CXXFLAGS)
ASFLAGS  := $(CFLAGS) $(ASFLAGS)
ARFLAGS  := rv

ifeq ($(USE_KERNEL_LIBC),1)
CFLAGS := -I$(PSPSDK)/include/libc $(CFLAGS)
else
ifeq ($(USE_PSPSDK_LIBC),1)
CFLAGS := -I$(PSPSDK)/include/libc $(CFLAGS)
endif
endif

ifdef WITH_JAPANESE
	CFLAGS += -DPB_SJIS_SUPPORT
	ifdef WITH_JIS_LEVEL2
		CFLAGS += -DPB_JIS_LEVEL2
	endif
endif

all: $(LIBPATH)/lib$(TARGET).a

$(LIBPATH)/lib$(TARGET).a: $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

clean:
	rm -rf $(LIBPATH)/lib$(TARGET).a $(OBJS)

rebuild: clean all
