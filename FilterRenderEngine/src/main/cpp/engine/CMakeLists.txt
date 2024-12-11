# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.
#CMakeLists.txt
cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

########################### set project name ####################################
set(TARGET_NAME filterRenderEngine)
project(${TARGET_NAME})

############################ set compile file ###################################
# get top path
string(REGEX REPLACE "/$" "" TOP_PATH ${CMAKE_CURRENT_SOURCE_DIR})
message("顶层目录=" $(TOP_PATH))

#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/glm)

# include director
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/base
        ${CMAKE_CURRENT_SOURCE_DIR}/glad
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/glm
        ${CMAKE_CURRENT_SOURCE_DIR}/util
        ${CMAKE_CURRENT_SOURCE_DIR}/render
)

# serach all cpp file and add to SRC_LIST
#aux_source_directory(${CMAKE_SOURCE_DIR} SRC_LIST)

#file(GLOB_RECURSE SRC_LIST "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB SRC_LIST
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/eglcore/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/render/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/util/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/glm/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/glm/detail/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/glm/gtc/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/glm/gtx/*.cpp)

#add to library
add_library(${TARGET_NAME} SHARED ${SRC_LIST})

############################ add dependent library ##############################
find_library( # Sets the name of the path variable.
        log-lib
        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log
        android)

#add dependent library

############################ set library generation #############################
target_link_libraries(${TARGET_NAME}
        ${log-lib}
        -ljnigraphics
        EGL
        GLESv3
        android)
