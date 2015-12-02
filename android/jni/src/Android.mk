LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL2
SDLTTF_PATH := ../SDL2_ttf
SDLMIXER_PATH := ../SDL2_mixer
ZZIP_PATH := ../zzip
GW_PATH := ../../..

LOCAL_CFLAGS += -DGW_PLAT_ANDROID -DGW_DEBUG -DEXCLUDEGW_USE_ZDATA
LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include $(LOCAL_PATH)/$(SDLTTF_PATH) $(LOCAL_PATH)/$(SDLMIXER_PATH) \
	$(LOCAL_PATH)/$(ZZIP_PATH)/include $(LOCAL_PATH)/$(GW_PATH)/include

# Add your application source files here...
LOCAL_SRC_FILES := $(SDL_PATH)/src/main/android/SDL_android_main.c \
	$(GW_PATH)/src/device.cpp \
	$(GW_PATH)/src/gamelist.cpp \
	$(GW_PATH)/src/gamewatch.cpp \
	$(GW_PATH)/src/gwmenu.cpp \
	$(GW_PATH)/src/platform.cpp \
	$(GW_PATH)/src/plat/plat_sdl.cpp \
	$(GW_PATH)/src/plat/SDL_rwops_zzip.cpp \
	$(GW_PATH)/src/plat/plat_android.cpp \
	$(GW_PATH)/src/devices/deveng_vtech.cpp \
	$(GW_PATH)/src/devices/deveng_vtech_banana.cpp \
	$(GW_PATH)/src/devices/deveng_vtech_condor.cpp \
	$(GW_PATH)/src/devices/deveng_vtech_monkey.cpp \
	$(GW_PATH)/src/devices/dev_banana.cpp \
	$(GW_PATH)/src/devices/dev_condor.cpp \
	$(GW_PATH)/src/devices/dev_defendo.cpp \
	$(GW_PATH)/src/devices/dev_monkey.cpp \
	$(GW_PATH)/src/devices/dev_pancake.cpp \
	$(GW_PATH)/src/devices/dev_pirate.cpp \
	$(GW_PATH)/src/devices/dev_rollerc.cpp \
	$(GW_PATH)/src/util/anyoption.cpp

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_mixer SDL2_ttf zzip

LOCAL_LDLIBS := -lGLESv1_CM -llog -lz

#APP_LIB_DEPENDS := $(foreach LIB, $(LOCAL_SHARED_LIBRARIES), $(abspath $(LOCAL_PATH)/../../obj/local/armeabi/lib$(LIB).so))

include $(BUILD_SHARED_LIBRARY)
