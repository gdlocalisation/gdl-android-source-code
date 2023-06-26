#include "includes.hpp"
#include <jni.h>
#include "hooks.hpp"

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    logD("GDL loading");
    Hooks::initMem();
    logD("GDL loaded");

    return JNI_VERSION_1_6;
}