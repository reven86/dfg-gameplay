
file(GLOB_RECURSE PROJECT_SRC *.cpp *.cc)

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

if(IOS)
    list(APPEND PROJECT_SRC
        iRate/iRate.m
    )
    set_source_files_properties(iRate/iRate.m PROPERTIES
        COMPILE_FLAGS "-x objective-c++ -fobjc-arc"
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
