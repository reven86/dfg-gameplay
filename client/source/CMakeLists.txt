
file(GLOB_RECURSE DFGGAMEPLAY_SRC *.cpp *.cc)

set(UUIDLIB_SRC
    uuidlib/clear.c
    uuidlib/compare.c
    uuidlib/copy.c
    uuidlib/gen_uuid.c
    uuidlib/isnull.c
    uuidlib/pack.c
    uuidlib/parse.c
    uuidlib/unpack.c
    uuidlib/unparse.c
    uuidlib/uuid_time.c
)

include_directories(
    base
    base/..
    ../../../GamePlay/gameplay/src
    ../../../GamePlay/external-deps/include
)

IF(LINUX)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK2 REQUIRED gtk+-2.0)
include_directories(${GTK2_INCLUDE_DIRS})
add_definitions(${GTK2_CFLAGS_OTHER})
add_definitions(-D__linux__)
ENDIF(LINUX)

add_definitions(-DGP_USE_SOCIAL)
add_definitions(-DGP_USE_STOREFRONT)
add_definitions(-DCURL_STATICLIB)
add_definitions(-DYAML_DECLARE_STATIC)
IF(NOT WIN32)
    add_compile_options(-Wno-comment)
    add_compile_options(-Wno-logical-op-parentheses)
ENDIF(NOT WIN32)

add_compile_options(
    "$<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>"
)

if(EMSCRIPTEN)
add_definitions(-DHAVE_INTTYPES_H)
add_definitions(-DHAVE_UNISTD_H)
add_definitions(-DHAVE_ERRNO_H)
add_definitions(-DHAVE_NETINET_IN_H)
add_definitions(-DHAVE_SYS_IOCTL_H)
add_definitions(-DHAVE_SYS_MMAN_H)
add_definitions(-DHAVE_SYS_MOUNT_H)
add_definitions(-DHAVE_SYS_PRCTL_H)
add_definitions(-DHAVE_SYS_RESOURCE_H)
add_definitions(-DHAVE_SYS_SELECT_H)
add_definitions(-DHAVE_SYS_STAT_H)
add_definitions(-DHAVE_SYS_TYPES_H)
add_definitions(-DHAVE_STDLIB_H)
add_definitions(-DHAVE_STRDUP)
add_definitions(-DHAVE_MMAP)
add_definitions(-DHAVE_UTIME_H)
add_definitions(-DHAVE_GETPAGESIZE)
add_definitions(-DHAVE_LSEEK64)
add_definitions(-DHAVE_LSEEK64_PROTOTYPE)
add_definitions(-DHAVE_EXT2_IOCTLS)
add_definitions(-DHAVE_LINUX_FD_H)
add_definitions(-DHAVE_TYPE_SSIZE_T)
add_definitions(-DHAVE_SYS_TIME_H)
add_definitions(-DHAVE_SYS_PARAM_H)
add_definitions(-DHAVE_SYSCONF)
endif(EMSCRIPTEN)

if(EMSCRIPTEN)
    set(PROJECT_SRC ${DFGGAMEPLAY_SRC} ${UUIDLIB_SRC})
else(EMSCRIPTEN)
    set(PROJECT_SRC ${DFGGAMEPLAY_SRC})
endif(EMSCRIPTEN)

if(IOS)
    list(APPEND PROJECT_SRC
        iRate/iRate.m
    )
    set_source_files_properties(iRate/iRate.m PROPERTIES
        COMPILE_FLAGS "-x objective-c++"
    )
    set_source_files_properties(base/game_advanced.cpp PROPERTIES
        COMPILE_FLAGS "-x objective-c++"
    )
    set_source_files_properties(base/main.cpp PROPERTIES
        COMPILE_FLAGS "-x objective-c++"
    )
endif(IOS)


add_library(dfg-gameplay STATIC
    ${PROJECT_SRC}
)

IF(ANDROID)
    target_include_directories(dfg-gameplay PRIVATE "${ANDROID_NDK}/sources/android/native_app_glue/")
ENDIF(ANDROID)

#target_precompile_headers(dfg-gameplay PRIVATE pch.h)

set_target_properties(dfg-gameplay PROPERTIES
    OUTPUT_NAME "dfg-gameplay"
    CLEAN_DIRECT_OUTPUT 1
)

source_group(src FILES ${PROJECT_SRC})
