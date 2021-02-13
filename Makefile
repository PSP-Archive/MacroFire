#======================================
# MacroFireをビルドするためのMakefile.
# 
#   使用可能なmakeターゲット:
#
#   all(default) - MacroFire本体を作成する
#   clean        - MacroFire本体とオブジェクトファイルを削除
#   macrofire    - allの別名
#
#   libs         - MacroFireをビルドするのに必要なライブラリを作成する
#   libsclean    - ライブラリ本体とオブジェクトファイルを削除
#
#   everything   - libs + macrofire
#   allclean     - clean + libsclean
#
#--------------------------------------
# 設定可能なオプション
#
# 日本語フォントと日本語表示サポート:
#     WITH_JAPANESE=1
#
# 日本語フォントのJIS第二水準までサポート:
#     WITH_JAPANESE=1 WITH_JIS_LEVEL2=1
#
# * 日本語フォントを使用する場合は、
# * MacroFire本体だけでなくライブラリも同じオプションをつけてビルドする必要があります。
# * そうでなければダイアログの表示がおかしくなるでしょう。
# *   make WITH_JAPANESE=1 allclean everything
#
#--------------------------------------
# MacroFire本体にのみ影響のあるオプション
#
# 例外ハンドラを設定する:
#     WITH_EXCEPTION_HANDLER=1
#
# PSPLINK用にデバッグメッセージをprintf出力する:
#     WITH_PSPLINK_DEBUG=1
#
# メインスレッド起動時に、指定したサイズを残してメモリを全て確保し、
# 空きメモリの少ない状態をシミュレート:
#     WITH_LOW_MEMORY_SIM=<FreeSize>
#
#======================================

TARGET = macrofire
PSP_FW_VERSION = 380

# PRXを作成する
BUILD_PRX = 1

# エクスポート関数定義ファイル
PRX_EXPORTS = exports.exp

# 機能が制限されているが軽量なカーネルlibc(sysclib)。
USE_KERNEL_LIBC = 1

# カーネルモードPRXを作る場合は推奨されているようだ。
# これを指定するといくつかのライブラリは
# 優先的にカーネルモードのライブラリ(psp***_driver)をリンクしようとするみたい。
USE_KERNEL_LIBS = 1

BASEPATH = .

INCDIR  = $(BASEPATH)/include
LIBDIR  = $(BASEPATH)/lib

CFLAGS      = -Os -G0 -Wall -fno-strict-aliasing
ASFLAGS     = $(CFLAGS)
LDFLAGS     = 
LIBS        = -lpspumd -lpsppower_driver -lpsphprm_driver -lpspge -lpsprtc -lpsputility -lm
CLASSG_LIBS = -lcdialog -lovmsg -linimgr -ldirh -lfiomgr -lpadctrl -lpadutil -lpb -ldmem_kernel -lheap_kernel -lmemory -lmakepath -lstrutil -lhook

OBJS = \
	exports.o \
	mfmain.o \
	$(patsubst %.S,%.o,$(wildcard psplib/*.S)) \
	$(patsubst %.c,%.o,$(wildcard mfctrl/*.c)) \
	$(patsubst %.c,%.o,$(wildcard mfdialog/*.c)) \
	$(patsubst %.c,%.o,$(wildcard mfanalogstick/*.c)) \
	$(patsubst %.c,%.o,$(wildcard mfrapidfire/*.c)) \
	$(patsubst %.c,%.o,$(wildcard mfmenu/*.c)) \
	$(patsubst %.c,%.o,$(wildcard functions/*.c))


LIBS := $(CLASSG_LIBS) $(LIBS)

ifdef WITH_LOW_MEMORY_SIM
	CFLAGS += -DMF_LOW_MEMORY_TEST=$(WITH_LOW_MEMORY_SIM)
endif

ifdef WITH_PSPLINK_DEBUG
	CFLAGS += -fno-builtin-printf -g -DDEBUG=1
	WITH_LIBS_DEBUG=1
endif

ifdef WITH_EXCEPTION_HANDLER
	CFLAGS += -DMF_WITH_EXCEPTION_HANDLER -g
	WITH_LIBS_DEBUG=1
endif

ifdef WITH_JAPANESE
	CFLAGS += -DPB_SJIS_SUPPORT
	ifdef WITH_JIS_LEVEL2
		CFLAGS += -DPB_JIS_LEVEL2
	endif
endif

ifeq ($(USE_KERNEL_LIBC),1)
    CFLAGS += -DPSP_USE_KERNEL_LIBC
	LIBS += -lsysclib
else
	LIBS += -lpspkernel -lc -lpspuser
endif

export WITH_LIBS_DEBUG
export USE_KERNEL_LIBC
export USE_KERNEL_LIBS

PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak

everything:
	@-rm $(TARGET).elf $(TARGET).prx
	@make libs
	@make $(TARGET)

allclean: libsclean clean

$(TARGET): all

libs:
	@$(MAKE) -C $(BASEPATH)/include

libsclean:
	@$(MAKE) -C $(BASEPATH)/include clean
