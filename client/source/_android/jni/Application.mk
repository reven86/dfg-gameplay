# Application.mk

APP_OPTIM := release
APP_MODULES := dfg-gameplay
APP_CPPFLAGS += -std=c++20 -fvisibility=hidden -fvisibility-inlines-hidden
APP_STL      := c++_static
APP_ABI     := armeabi-v7a arm64-v8a x86 x86_64