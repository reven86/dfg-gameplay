cmake_minimum_required(VERSION 3.10...3.27)

project(dfg-gameplay)

set(CMAKE_CXX_STANDARD 17)


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

message("Platform: ${CMAKE_SYSTEM_NAME}")

# ARCH detection
if (ANDROID)
    set(ARCH ${ANDROID_ARCH_NAME})
elseif(IOS)
    if (PLATFORM STREQUAL "OS64")
        set(ARCH "arm")
    else (PLATFORM STREQUAL "OS64")
        set(ARCH "x86")
    endif (PLATFORM STREQUAL "OS64")
else(IOS)
    # Skip any introspection for cross-compiling targets
    include(${PROJECT_SOURCE_DIR}/../../GamePlay/cmake/TargetArch.cmake)
    target_architecture(ARCH)
endif(ANDROID)

string(TOLOWER ${CMAKE_SYSTEM_NAME} LOWER_SYSTEM_NAME)
set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/_out")
set(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/_out/lib/${LOWER_SYSTEM_NAME}/${ARCH}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH})

file(MAKE_DIRECTORY ${LIBRARY_OUTPUT_PATH})
message(STATUS "Directory: ${LIBRARY_OUTPUT_PATH}")

if (GP_NO_UI)
add_definitions(-DGP_NO_UI=1)
endif (GP_NO_UI)

# library
add_subdirectory(source)