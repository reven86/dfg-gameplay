# Application.mk

APP_OPTIM := release
APP_MODULES := dfg-gameplay
APP_CPPFLAGS += -fvisibility=hidden -fvisibility-inlines-hidden -std=gnu++17
APP_ABI     := armeabi-v7a arm64-v8a x86 x86_64