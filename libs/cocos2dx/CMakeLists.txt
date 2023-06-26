cmake_minimum_required(VERSION 3.0.0)
project(cocos-headers CXX)

add_library(cocos2d INTERFACE)

target_include_directories(cocos2d INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/Includes"
    "${CMAKE_CURRENT_SOURCE_DIR}/kazmath/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/platform/android"
    "${CMAKE_CURRENT_SOURCE_DIR}/support"
    "${CMAKE_CURRENT_SOURCE_DIR}/"
    "${CMAKE_CURRENT_SOURCE_DIR}/base_nodes"
    "${CMAKE_CURRENT_SOURCE_DIR}/extensions/DS_Dictionary"
    "${CMAKE_CURRENT_SOURCE_DIR}/platform"
    "${CMAKE_CURRENT_SOURCE_DIR}/extensions"
)

target_link_libraries(cocos2d INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/../libcocos2dcpp.so)
target_compile_definitions(cocos2d INTERFACE ANDROID)