
set(DFGGAMEPLAY_SRC
    pch.cpp
    base/main/archive.cpp
    base/main/asset.cpp
    base/main/dictionary.cpp
    base/main/gameplay_assets.cpp
    base/main/memory_stream.cpp
    base/main/settings.cpp
    base/main/variant.cpp
    base/main/zip_package.cpp
    base/main/zip_packages.cpp
    base/main/zip_stream.cpp
    base/render/particle_system.cpp
    base/services/debug_service.cpp
    base/services/httprequest_service.cpp
    base/services/input_service.cpp
    base/services/render_service.cpp
    base/services/service.cpp
    base/services/service_manager.cpp
    base/services/social_service.cpp
    base/services/storefront_service.cpp
    base/services/taskqueue_service.cpp
    base/services/taskscheduler_service.cpp
    base/services/tracker_service.cpp
    base/ui/carousel.cpp
    base/ui/clip_label.cpp
    base/ui/control_event_handler.cpp
    base/ui/dial_button.cpp
    base/ui/expanded_tab.cpp
    base/ui/http_image_control.cpp
    base/ui/slide_menu.cpp
    base/utils/profiler.cpp
    base/utils/singleton.cpp
    base/utils/utils.cpp
    base/main.cpp
    base/game_advanced.cpp
)

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

set_source_files_properties(${DFGGAMEPLAY_SRC} PROPERTIES COMPILE_FLAGS "-std=c++11")

add_definitions(-lstdc++)
add_definitions(-DGP_USE_SOCIAL)
add_definitions(-DGP_USE_STOREFRONT)
add_definitions(-DCURL_STATICLIB)
add_definitions(-DZIP_STATIC)
add_definitions(-DYAML_DECLARE_STATIC)
IF(NOT WIN32)
add_definitions(-Wno-logical-op-parentheses)
ENDIF(NOT WIN32)

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

add_library(dfg-gameplay STATIC
    ${PROJECT_SRC}
)

set_target_properties(dfg-gameplay PROPERTIES
    OUTPUT_NAME "dfg-gameplay"
    CLEAN_DIRECT_OUTPUT 1
)

source_group(src FILES ${PROJECT_SRC})



