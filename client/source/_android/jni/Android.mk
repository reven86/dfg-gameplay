
# Copyright (C) 2010 The Android Open Source Project

#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

SAMPLE_PATH := $(call my-dir)/../../
GAMEPLAY_LOCAL_PATH = ../../../GamePlay/

GAMEPLAY_PATH := $(SAMPLE_PATH)$(GAMEPLAY_LOCAL_PATH)
GAMEPLAY_DEPS := $(SAMPLE_PATH)$(GAMEPLAY_LOCAL_PATH)external-deps/lib/android/$(TARGET_ARCH_ABI)

# dfg-gameplay
LOCAL_PATH := $(SAMPLE_PATH)
include $(CLEAR_VARS)

LOCAL_MODULE    := libdfg-gameplay
LOCAL_PCH := pch.h
LOCAL_SRC_FILES := \
    pch.cpp \
    uuidlib/clear.c \
    uuidlib/compare.c \
    uuidlib/copy.c \
    uuidlib/gen_uuid.c \
    uuidlib/isnull.c \
    uuidlib/pack.c \
    uuidlib/parse.c \
    uuidlib/unpack.c \
    uuidlib/unparse.c \
    uuidlib/uuid_time.c \
    base/entity/entity.cpp \
    base/entity/entity_manager.cpp \
    base/main/archive.cpp \
    base/main/asset.cpp \
    base/main/dictionary.cpp \
    base/main/gameplay_assets.cpp \
    base/main/memory_stream.cpp \
    base/main/settings.cpp \
    base/main/socket_stream.cpp \
    base/main/variant.cpp \
    base/main/zip_packages.cpp \
    base/main/zip_stream.cpp \
    base/render/particle_system.cpp \
    base/services/debug_service.cpp \
    base/services/httprequest_service.cpp \
    base/services/input_service.cpp \
    base/services/render_service.cpp \
    base/services/service.cpp \
    base/services/service_manager.cpp \
    base/services/social_service.cpp \
    base/services/storefront_service.cpp \
    base/services/taskqueue_service.cpp \
    base/services/taskscheduler_service.cpp \
    base/services/tracker_service.cpp \
    base/ui/carousel.cpp \
    base/ui/clip_label.cpp \
    base/ui/control_event_handler.cpp \
    base/ui/dial_button.cpp \
    base/ui/expanded_tab.cpp \
    base/ui/http_image_control.cpp \
    base/ui/slide_menu.cpp \
    base/utils/profiler.cpp \
    base/utils/singleton.cpp \
    base/utils/utils.cpp \
    base/utils/run_on_change.cpp \
    base/main.cpp \
    base/game_advanced.cpp \

# LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv2 -lOpenSLES
LOCAL_CFLAGS    := -O3 -D__ANDROID__ -fpermissive -fexceptions \
    -I"$(GAMEPLAY_PATH)external-deps/include" \
    -I"$(SAMPLE_PATH)base" \
    -I"$(GAMEPLAY_PATH)gameplay/src" \
    -DHAVE_INTTYPES_H \
    -DHAVE_UNISTD_H \
    -DHAVE_ERRNO_H \
    -DHAVE_NETINET_IN_H \
    -DHAVE_SYS_IOCTL_H \
    -DHAVE_SYS_MMAN_H \
    -DHAVE_SYS_MOUNT_H \
    -DHAVE_SYS_PRCTL_H \
    -DHAVE_SYS_RESOURCE_H \
    -DHAVE_SYS_SELECT_H \
    -DHAVE_SYS_STAT_H \
    -DHAVE_SYS_TYPES_H \
    -DHAVE_STDLIB_H \
    -DHAVE_STRDUP \
    -DHAVE_MMAP \
    -DHAVE_UTIME_H \
    -DHAVE_GETPAGESIZE \
    -DHAVE_LSEEK64 \
    -DHAVE_LSEEK64_PROTOTYPE \
    -DHAVE_EXT2_IOCTLS \
    -DHAVE_LINUX_FD_H \
    -DHAVE_TYPE_SSIZE_T \
    -DHAVE_SYS_TIME_H \
    -DHAVE_SYS_PARAM_H \
    -DHAVE_SYSCONF \
    -D_GLIBCXX_USE_C99_STDINT_TR1 \
    -DGP_USE_SOCIAL \
    -DGP_USE_STOREFRONT \

LOCAL_CPPFLAGS := -frtti -Wno-switch-enum -Wno-switch -Wno-logical-op-parentheses
LOCAL_STATIC_LIBRARIES := android_native_app_glue
LOCAL_ARM_MODE := arm


#include $(BUILD_SHARED_LIBRARY)            # uncomment this line to build a shared library
include $(BUILD_STATIC_LIBRARY)           # here, we are building a static library
$(call import-module,android/native_app_glue)
