#pragma once
#include "fmt/format.h"
#include "android/log.h"

#define MODNAME "GDL"

// #define GDL_DEBUG

template <typename... Args>
void logD(Args... args) {
#ifdef GDL_DEBUG
    auto str = fmt::format(args...);
    __android_log_print(ANDROID_LOG_DEBUG, MODNAME, "%s", str.c_str());
#endif
}

template <typename... Args>
void logW(Args... args) {
#ifdef GDL_DEBUG
    auto str = fmt::format(args...);
    __android_log_print(ANDROID_LOG_WARN, MODNAME, "%s", str.c_str());
#endif
}

template <typename... Args>
void logE(Args... args) {
#ifdef GDL_DEBUG
    auto str = fmt::format(args...);
    __android_log_print(ANDROID_LOG_ERROR, MODNAME, "%s", str.c_str());
#endif
}