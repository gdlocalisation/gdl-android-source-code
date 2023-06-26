cmake_minimum_required(VERSION 3.0.0)

# write path to ur android ndk into NDKPath.txt. Recommended version is r16b
# use / in the path (not \ or \\)
file(STRINGS "NDKPath.txt" CMAKE_ANDROID_NDK)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/out)
set(CMAKE_SYSTEM_NAME Android)
set(CMAKE_SYSTEM_VERSION 21)
set(CMAKE_ANDROID_ARCH_ABI armeabi-v7a) # arm64-v8a / armeabi-v7a
set(CMAKE_ANDROID_STL_TYPE gnustl_static)
set(MODNAME GDL)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

project(${MODNAME} VERSION 1.0.0)

file(GLOB SOURCES
    src/*.cpp
    libs/KittyMemory/*.cpp
)

add_subdirectory(libs/fmt)
add_subdirectory(libs/cocos2dx)
add_subdirectory(libs/utf8)
set(JANSSON_BUILD_DOCS OFF CACHE INTERNAL "" FORCE)
add_subdirectory(libs/jansson)

add_library(${MODNAME} SHARED ${SOURCES})

target_link_libraries(${MODNAME} log ${CMAKE_SOURCE_DIR}/libs/dobby/libdobby.a fmt cocos2d utf8cpp jansson ${CMAKE_SOURCE_DIR}/libs/libcocos2dcpp.so)
target_include_directories(${MODNAME} PUBLIC
    libs/dobby
    libs/KittyMemory
    libs/jansson/src
    libs/jansson/android
    libs
    libs/gdh/
    libs/gdh/achievement_nodes
    libs/gdh/actions
    libs/gdh/audio_nodes
    libs/gdh/delegates
    libs/gdh/include
    libs/gdh/layers_scenes_transitions_nodes
    libs/gdh/level_nodes
    libs/gdh/manager_nodes
    libs/gdh/menu_nodes
    libs/gdh/other_nodes
    libs/gdh/scroll_nodes
    libs/gdh/sprite_nodes
    libs/gdh/text_input_nodes
)

install(TARGETS ${MODNAME} LIBRARY)