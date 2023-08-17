
NDK_DIR = D:/android-ndk-r11c/
TOOLCHAIN_DIR = $(NDK_DIR)toolchains/arm-linux-androideabi-4.9/prebuilt/windows-x86_64/bin/
GCC = $(TOOLCHAIN_DIR)arm-linux-androideabi-g++
PLAT_DIR = $(NDK_DIR)platforms/android-9/arch-arm/usr/

LINK_LIBS = -l:libEGL_mtk.so -l:libGLESv1_CM_mtk.so

SRC_APPS = apps/dialer.cpp
SRC_MODULES = main.cpp modules/pm.cpp modules/animator.cpp modules/input.cpp modules/status.cpp modules/display.cpp modules/modem.cpp modules/app.cpp modules/gui.cpp

OUTPUT_NAME = cmdprog
CPP = main.cpp modules/modem.cpp

#$(GCC) -I $(PLAT_DIR)include/ -L $(PLAT_DIR)lib/ -fPIE -Wl,-dynamic-linker=/sbin/linker $(LINK_LIBS) -static -o $(OUTPUT_NAME) main.cpp micro2d.cpp
compile:
	$(GCC) -I $(PLAT_DIR)include/ -I include/ -L $(PLAT_DIR)lib/ -fno-rtti -fpermissive -fno-exceptions -Wno-write-strings -Wl,-dynamic-linker=/system/bin/linker  -static -o build/mono $(SRC_MODULES) $(SRC_APPS) 