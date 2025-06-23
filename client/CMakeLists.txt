cmake_minimum_required(VERSION 3.1)

project(dfg-gameplay)

set(CMAKE_CXX_STANDARD 20)


# debug
message( "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}" )
if ( "${CMAKE_BUILD_TYPE}" STREQUAL "DEBUG" )
    add_definitions(-D_DEBUG)
endif()

IF(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND NOT ANDROID)
    SET(LINUX 1)
ENDIF(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND NOT ANDROID)

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten")
    SET(EMSCRIPTEN 1)
endif("${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten")

if (NOT "${CMAKE_SYSTEM_NAME_OVERRIDE}" STREQUAL "")
    SET(CMAKE_SYSTEM_NAME "${CMAKE_SYSTEM_NAME_OVERRIDE}")
endif (NOT "${CMAKE_SYSTEM_NAME_OVERRIDE}" STREQUAL "")

message("Platform: ${CMAKE_SYSTEM_NAME}")

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

message("Architecture: ${ARCH}")

# gameplay library
add_subdirectory(source)