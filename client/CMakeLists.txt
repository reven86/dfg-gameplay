cmake_minimum_required(VERSION 2.8)

project(dfg-gameplay)

set(CMAKE_C_COMPILER_INIT g++)

# debug
message( "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}" )
if ( "${CMAKE_BUILD_TYPE}" STREQUAL "DEBUG" )
    add_definitions(-D_DEBUG)
endif()

IF(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND NOT ANDROID)
    SET(LINUX 1)
ENDIF(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND NOT ANDROID)

if(NOT ANDROID AND NOT IOS)
    # Skip any introspection for cross-compiling targets
    include(${PROJECT_SOURCE_DIR}/cmake/TargetArch.cmake)

    set(ARCH "unknown")
    target_architecture(ARCH)
endif(NOT ANDROID AND NOT IOS)

set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/_out")

if(NOT ANDROID AND NOT IOS)
    # Android and iOS toolchains will set their own output path
    IF(APPLE)
        set(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/_out/lib/macosx/${ARCH}")
    ELSE()
        string(TOLOWER ${CMAKE_SYSTEM_NAME} LOWER_SYSTEM_NAME)
        set(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/_out/lib/${LOWER_SYSTEM_NAME}/${ARCH}")
    ENDIF(APPLE)
endif(NOT ANDROID AND NOT IOS)

# gameplay library
add_subdirectory(source)