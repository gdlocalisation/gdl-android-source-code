#pragma once
#include "includes.hpp"
#include <dlfcn.h>
#include <dirent.h>
#include <regex>

#define GDL_DEBUG

#define HOOK_FUNC(retType, name, ...)                                                                                                      \
    retType (*name##_o)(__VA_ARGS__);                                                                                                      \
    retType name##_hk(__VA_ARGS__)

// member by offset
#define MBO(type, obj, offset) *(type*)((uintptr_t)obj + offset)

inline void hook(const string& sym, void* hk, void** tramp, const char* libName = "libcocos2dcpp.so") {
    logD("Hooked {} of {} with code {}", sym, libName, DobbyHook(dlsym(dlopen(libName, RTLD_LAZY), sym.c_str()), hk, tramp));
}

inline string removeTags(string str, bool removeColors = true, bool removeDelay = true, bool removeInstant = true) {
    if (removeColors) {
        str = std::regex_replace(str, std::regex("<c.>"), ""); // no starting <cX>
        str = std::regex_replace(str, std::regex("</c>"), ""); // no ending </c>
    }
    if (removeDelay)
        str = std::regex_replace(str, std::regex("<d...>"), ""); // no <dXXX>
    if (removeInstant) {
        str = std::regex_replace(str, std::regex("<i>"), "");  // no starting <i>
        str = std::regex_replace(str, std::regex("</i>"), ""); // no ending </i>
    }

    return str;
}

// #define PATCH_DEBUG

inline void patch(uintptr_t absAddr, uint8_t* bytes, size_t len) {
#ifdef PATCH_DEBUG
    logD("Written {} byte(s) to 0x{:X} with code {}", len, absAddr, DobbyCodePatch((void*)absAddr, bytes, len));
#else
    DobbyCodePatch((void*)absAddr, bytes, len);
#endif
}

inline void patch(const ProcMap& map, uintptr_t relAddr, uint8_t* bytes, size_t len) {
    patch(map.startAddress + relAddr, bytes, len);
}

inline void patch(const string& libName, uintptr_t relAddr, uint8_t* bytes, size_t len) {
    auto maps = KittyMemory::getMapsByName(libName);
    if (maps.size() > 0)
        patch(maps[0].startAddress + relAddr, bytes, len);
    // patch(maps[0], relAddr, bytes, len);
    else
        logE("Failed to find a memory map by name {}", libName);
}

inline vector<string> splitString(string str, char separator) {
    string temp = "";
    vector<string> v;

    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == separator) {
            v.push_back(temp);
            temp = "";
        } else {
            temp.push_back(str[i]);
        }
    }
    v.push_back(temp);

    return v;
}

inline string joinStrings(vector<string> strings, const char* delim) {
    string ret;

    for (size_t i = 0; i < strings.size(); i++) {
        auto str = strings[i];
        ret += (i != strings.size() - 1) ? str + delim : str;
    }

    return ret;
}

inline string replaceUnicode(string str) {
    string ret = "";

    auto b = str.begin();
    auto e = str.end();
    while (b != e) {
        auto cp = utf8::next(b, e);
        ret += isascii(cp) ? (char)cp : 'E'; // it can be any 1-byte char so it doesnt mess up with counting symbols
    }

    return ret;
}

inline void processColors(string str, CCArray* letters) {
    str = replaceUnicode(str);

    // colors
    string modStr = removeTags(str, false);

    int pos = 0;
    while (true) {
        int beginPos = modStr.find("<c", pos);
        if (beginPos == string::npos)
            break;

        auto tag = modStr.at(beginPos + 2);

        size_t endPos = modStr.find("</c>", pos);

        ccColor3B col;

        // thx to Wylie https://github.com/Wyliemaster/GD-Decompiled/blob/main/GD/code/src/MultilineBitmapFont.cpp#L23
        switch (tag) {
        case 'b':
            col = {0x4A, 0x52, 0xE1};
            break;
        case 'g':
            col = {0x40, 0xE3, 0x48};
            break;
        case 'l':
            col = {0x60, 0xAB, 0xEF};
            break;
        case 'j':
            col = {0x32, 0xC8, 0xFF};
            break;
        case 'y':
            col = {0xFF, 0xFF, 0x00};
            break;
        case 'o':
            col = {0xFF, 0xA5, 0x4B};
            break;
        case 'r':
            col = {0xFF, 0x5A, 0x5A};
            break;
        case 'p':
            col = {0xFF, 0x00, 0xFF};
            break;
        default:
            col = {0xFF, 0x00, 0x00};
        }

        for (size_t i = beginPos; i < endPos - 4; i++) {
            if (i >= letters->count())
                break;

            dynamic_cast<CCSprite*>(letters->objectAtIndex(i))->setColor(col);
        }

        modStr.erase(modStr.begin() + beginPos, modStr.begin() + beginPos + 4); // remove the <cX>
        modStr.erase(modStr.begin() + endPos - 4, modStr.begin() + endPos);     // remove the </c>

        pos = beginPos + 1;
    }

    // delay
    modStr = removeTags(str, true, false);

    pos = 0;
    while (true) {
        size_t beginPos = modStr.find("<d", pos);
        if (beginPos == string::npos)
            break;

        float delay = (atoi(modStr.substr(beginPos + 2, 5).c_str())) / 100.0f;

        if (beginPos < letters->count()) {
            // dynamic_cast<CCSprite*>(letters->objectAtIndex(beginPos))->m_fDelay = delay;
            auto obj = letters->objectAtIndex(beginPos);
            MBO(float, obj, 480) = delay;
        }

        modStr.erase(modStr.begin() + beginPos, modStr.begin() + beginPos + 6);

        pos = beginPos + 1;
    }

    // instant
    modStr = removeTags(str, true, true, false);

    pos = 0;
    while (true) {
        size_t beginPos = modStr.find("<i>", pos);
        if (beginPos == string::npos)
            break;

        size_t endPos = modStr.find("</i>", pos);

        for (size_t i = beginPos; i < endPos - 3; i++) {
            if (i >= letters->count())
                break;

            dynamic_cast<CCSprite*>(letters->objectAtIndex(i))->setVisible(true);
        }

        modStr.erase(modStr.begin() + beginPos, modStr.begin() + beginPos + 3); // remove the <i>
        modStr.erase(modStr.begin() + endPos - 3, modStr.begin() + endPos);     // remove the </i>

        pos = beginPos + 1;
    }
}

inline json_t* loadJson(const string& name) {
    auto utils = CCFileUtils::sharedFileUtils();
    auto path = utils->fullPathForFilename(name.c_str(), false);
    unsigned long size = 0;
    auto contents = utils->getFileData(path.c_str(), "rb", &size);
    if (!contents)
        return nullptr;

    json_error_t err;
    auto ret = json_loadb((const char*)contents, size, 0, &err);
    delete[] contents;
    return ret;
}

inline bool shouldReverseGauntlet(int id) {
    return id == 4 ||  // Shadow
           id == 5 ||  // Lava
           id == 7 ||  // Chaos
           id == 9 ||  // Time
           id == 11 || // Magic
           id == 12 || // Spike
           id == 13 || // Monster
           id == 14 || // Doom
           id == 15;   // Death
}