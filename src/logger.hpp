#pragma once
#include "fmt/format.h"
#include "android/log.h"

#ifndef MODNAME
#define MODNAME "GDL"
#endif

template <typename... Args>
void logD(Args... args) {
    auto str = fmt::format(args...);
    __android_log_print(ANDROID_LOG_DEBUG, MODNAME, "%s", str.c_str());
}

template <typename... Args>
void logW(Args... args) {
    auto str = fmt::format(args...);
    __android_log_print(ANDROID_LOG_WARN, MODNAME, "%s", str.c_str());
}

template <typename... Args>
void logE(Args... args) {
    auto str = fmt::format(args...);
    __android_log_print(ANDROID_LOG_ERROR, MODNAME, "%s", str.c_str());
}