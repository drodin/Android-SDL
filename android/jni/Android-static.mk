LOCAL_PATH := $(call my-dir)/../..

include $(CLEAR_VARS)

LOCAL_MODULE := sdl

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/include \

SDL_SRCS := \
    src/*.c \
    src/audio/*.c \
    src/cpuinfo/*.c \
    src/events/*.c \
    src/file/*.c \
    src/stdlib/*.c \
    src/thread/*.c \
    src/timer/*.c \
    src/video/*.c \
    src/thread/pthread/*.c \
    src/timer/unix/*.c \
    src/audio/android/*.c \
    src/video/android/*.c \

LOCAL_SRC_FILES := $(foreach F, $(SDL_SRCS), $(addprefix $(dir $(F)),$(notdir $(wildcard $(LOCAL_PATH)/$(F)))))

include $(BUILD_STATIC_LIBRARY)
