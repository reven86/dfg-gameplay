
set(DFGGAMEPLAY_SRC
    pch.cpp
    base/main/asset.cpp
    base/main/dictionary.cpp
    base/main/gameplay_assets.cpp
    base/main/memory_stream.cpp
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
    base/ui/control_event_handler.cpp
    base/ui/dial_button.cpp
    base/ui/expanded_tab.cpp
    base/utils/profiler.cpp
    base/utils/singleton.cpp
    base/utils/utils.cpp
    base/main.cpp
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
    ../../third_party/libsigc++/include
    ../../../GamePlay/gameplay/src
    ../../../GamePlay/external-deps/include
)

IF(CMAKE_SYSTEM_NAME MATCHES "Linux")
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK2 REQUIRED gtk+-2.0)
include_directories(${GTK2_INCLUDE_DIRS})
add_definitions(${GTK2_CFLAGS_OTHER})
add_definitions(-D__linux__)
ENDIF(CMAKE_SYSTEM_NAME MATCHES "Linux")

add_definitions(-std=c++11)
add_definitions(-lstdc++)
add_definitions(-DGP_USE_SOCIAL)
add_definitions(-DGP_USE_STOREFRONT)

add_library(dfg-gameplay STATIC
    ${DFGGAMEPLAY_SRC}
)

set_target_properties(dfg-gameplay PROPERTIES
    OUTPUT_NAME "dfg-gameplay"
    CLEAN_DIRECT_OUTPUT 1
)

source_group(src FILES ${DFGGAMEPLAY_SRC})



