#include "hooks.hpp"

namespace Hooks {
    string g_currentFont;
    json_t* g_locationsJson;

    HOOK_FUNC(bool, initWithFont, CCNode* self, const char* fontName, string str, float scale, float width, CCPoint anchorPoint, int unk,
              bool bColourDisabled) {

        g_currentFont = fontName;

        if (!initWithFont_o(self, fontName, str, scale, width, anchorPoint, unk, bColourDisabled))
            return false;

        if (!bColourDisabled) {
            auto letters = cocos2d::CCArray::create();
            for (int i = 0; i < self->getChildrenCount(); i++) {
                auto lbl = (CCNode*)(self->getChildren()->objectAtIndex(i));
                letters->addObjectsFromArray(lbl->getChildren());
            }

            processColors(str, letters);
        }

        return true;
    }

    HOOK_FUNC(string, readColorInfo, void* self, string str) {
        return removeTags(str);
    }

    HOOK_FUNC(string, stringWithMaxWidth, void* self, string str, float scaledWidth, float scale) {
        auto lbl = CCLabelBMFont::create("", g_currentFont.c_str());
        lbl->setScale(scale);

        auto hasNL = str.find("\n") != string::npos;
        auto line = hasNL ? splitString(str, '\n')[0] : str;

        float width = scaledWidth / CCDirector::sharedDirector()->getContentScaleFactor();

        bool overflown = false;
        string current;

        auto b = line.begin();
        auto e = line.end();
        while (b != e) {
            auto cp = utf8::next(b, e);
            utf8::append((char32_t)cp, current);

            lbl->setString(current.c_str());

            if (lbl->getScaledContentSize().width > width) {
                overflown = true;
                break;
            }
        }

        if (overflown) {
            if (current.find(' ') != string::npos) {
                auto words = splitString(current, ' ');
                words.pop_back();
                current = joinStrings(words, " ");
                current = joinStrings(words, " ") + " ";
            }
        } else if (hasNL) {
            current += " ";
        }

        return current;
    }

    void initPatches() {
        logD("initPatches");
        auto map = KittyMemory::getMapsByName("libcocos2dcpp.so")[0];

        static vector<const char*> strs;

        auto lang = loadJson("ru_ru.json");
        auto addrs = loadJson("gdl_addrs.json");

        strs.clear();
        strs.reserve(json_object_size(lang));

        void* n;
        const char* key;
        json_t* value;
        json_object_foreach_safe(lang, n, key, value) {
            auto addrsList = json_object_get(addrs, key);

            if (!addrsList) {
                continue;
            }

            strs.push_back(json_string_value(value));

            size_t i = 0;

            json_t* valDCD;
            json_array_foreach(json_array_get(addrsList, 0), i, valDCD) {
                patch(map, json_integer_value(valDCD), (uint8_t*)&strs[strs.size() - 1], 4);
            }

            json_t* valADD;
            json_array_foreach(json_array_get(addrsList, 1), i, valADD) {
                patch(map, json_integer_value(valADD), (uint8_t*)"\x00\xBF", 2);
            }
        }
    }

    HOOK_FUNC(void, openURL, void* self, const char* url) {
        if (string(url) == "http://robtopgames.com/blog/2017/02/01/geometry-dash-newgrounds")
            url = "https://gdlocalisation.netlify.app/gd/blog/ru/#newgrounds_start";
        else if (string(url) == "http://www.boomlings.com/files/GJGuide.pdf")
            url = "https://gdlocalisation.netlify.app/gd/gjguide/ru/gjguide_ru.pdf";
        else if (string(url) == "http://www.robtopgames.com/gd/faq")
            url = "https://gdlocalisation.netlify.app/gd/blog/ru";

        openURL_o(self, url);
    }

    HOOK_FUNC(bool, GauntletNode_init, CCNode* self, void* mapPack) {
        if (!GauntletNode_init_o(self, mapPack))
            return false;

        auto packID = MBO(int, mapPack, 240);
        if (shouldReverseGauntlet(packID)) {
            auto nameLabel = reinterpret_cast<CCNode*>(self->getChildren()->objectAtIndex(3));
            auto nameShadow = reinterpret_cast<CCNode*>(self->getChildren()->objectAtIndex(5));
            auto gauntletLabel = reinterpret_cast<CCNode*>(self->getChildren()->objectAtIndex(4));
            auto gauntletShadow = reinterpret_cast<CCNode*>(self->getChildren()->objectAtIndex(6));

            nameLabel->setPositionY(75);
            nameShadow->setPositionY(72);
            gauntletLabel->setPositionY(94);
            gauntletShadow->setPositionY(91);

            nameLabel->setScale(0.45f);
            nameShadow->setScale(0.45f);
            gauntletLabel->setScale(0.62f);
            gauntletShadow->setScale(0.62f);
        }

        return true;
    }

    HOOK_FUNC(bool, GauntletLayer_init, CCNode* self, int gauntletType) {
        if (!GauntletLayer_init_o(self, gauntletType))
            return false;

        CCLabelBMFont* nameLabel = nullptr;
        CCLabelBMFont* shadowLabel = nullptr;

        // you cant do it with class members or just getChildren()->objectAtIndex()
        for (size_t i = 0; i < self->getChildren()->count(); i++) {
            auto node = dynamic_cast<CCLabelBMFont*>(self->getChildren()->objectAtIndex(i));
            if (node) {
                if (nameLabel == nullptr)
                    nameLabel = node;
                else if (shadowLabel == nullptr)
                    shadowLabel = node;
            }
        }

        string gauntletName;
        reinterpret_cast<void (*)(string*, int)>(
            dlsym(dlopen("libcocos2dcpp.so", RTLD_LAZY), "_ZN12GauntletNode11nameForTypeE12GauntletType"))(&gauntletName, gauntletType);

        auto newName = fmt::format(shouldReverseGauntlet(gauntletType) ? "Остров {}" : "{} Остров", gauntletName);

        nameLabel->setString(newName.c_str());
        shadowLabel->setString(newName.c_str());

        return true;
    }

    HOOK_FUNC(bool, LevelLeaderboard_init, CCNode* self, void* lvl, int type) {
        if (!LevelLeaderboard_init_o(self, lvl, type))
            return false;

        CCLabelBMFont* lbl = nullptr;

        auto idk = reinterpret_cast<CCNode*>(self->getChildren()->objectAtIndex(0));
        for (size_t i = 0; i < idk->getChildrenCount(); i++) {
            auto node = dynamic_cast<CCLabelBMFont*>(idk->getChildren()->objectAtIndex(i));

            if (node)
                if (lbl == nullptr)
                    lbl = node;
        }

        lbl->setString(fmt::format("Таблица Лидеров для {}", MBO(string, lvl, 252)).c_str());

        return true;
    }

    HOOK_FUNC(void, customSetup, GJDropDownLayer* self) {
        customSetup_o(self);

        auto spr = cocos2d::CCSprite::createWithSpriteFrameName("gdl_icon.png");
        spr->setScale(1.25f);

        auto btn = CCMenuItemSpriteExtra::create(spr, spr, self, menu_selector(GDLMenu::openLayer));
        btn->setPosition({0, 0});

        auto menu = CCMenu::create();
        menu->addChild(btn, 99999);
        menu->setPosition({30, 30});
        self->m_pLayer->addChild(menu, 99999);
    }

    HOOK_FUNC(void, CCLabelBMFont_setScale, CCLabelBMFont* self, float scale) {
        auto entry = json_object_get(g_locationsJson, self->getString());
        if (entry) {
            auto scaleEntry = json_object_get(entry, "scale");
            if (scaleEntry)
                scale = json_real_value(scaleEntry);
        }

        CCLabelBMFont_setScale_o(self, scale);
    }

    // TODO: actually make it work
    // HOOK_FUNC(void, CCLabelBMFont_setPosition, CCLabelBMFont* self, const CCPoint& pos) {
    //     logD("cclabelbmfont setposition");
    //     logD("text set pos {} ; {} {}", self->getString(), pos.x, pos.y);
    //     CCPoint pos_ = pos;

    //     auto entry = json_object_get(g_locationsJson, self->getString());
    //     if (entry) {
    //         auto xEntry = json_object_get(entry, "x");
    //         if (xEntry)
    //             pos_.x += json_real_value(xEntry);

    //         auto yEntry = json_object_get(entry, "y");
    //         if (yEntry)
    //             pos_.y += json_real_value(yEntry);
    //     }

    //     CCLabelBMFont_setPosition_o(self, pos_);
    // }

    HOOK_FUNC(void, CCNode_setPos, CCNode* self, const CCPoint& pos) {
        auto lbl = dynamic_cast<CCLabelBMFont*>(self);

        if (!lbl)
            return CCNode_setPos_o(self, pos);

        CCPoint pos_ = pos;

        auto entry = json_object_get(g_locationsJson, lbl->getString());
        if (entry) {
            auto xEntry = json_object_get(entry, "x");
            if (xEntry)
                pos_.x += json_integer_value(xEntry);

            auto yEntry = json_object_get(entry, "y");
            if (yEntry)
                pos_.y += json_integer_value(yEntry);
        }

        CCNode_setPos_o(self, pos_);
    }

    HOOK_FUNC(bool, MenuLayer_init, CCNode* self) {
        if (!MenuLayer_init_o(self))
            return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        auto lbl = CCLabelBMFont::create("GDL 1.1.2", "goldFont.fnt");
        lbl->setPosition({winSize.width / 2.f, winSize.height - 15.f});
        lbl->setScale(.8f);
        self->addChild(lbl);

        return true;
    }

    HOOK_FUNC(bool, LoadingLayer_init, void* self, bool reload) {
        g_locationsJson = loadJson("ru_ru_locations.json");

        initPatches();

        logD("Loading GDL texture sheet");
        CCTextureCache::sharedTextureCache()->addImage(CCFileUtils::sharedFileUtils()->fullPathForFilename("GDL_Sheet.png", false).c_str(),
                                                       false);
        CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(
            CCFileUtils::sharedFileUtils()->fullPathForFilename("GDL_Sheet.plist", false).c_str());

        return LoadingLayer_init_o(self, reload);
    }

    void initMem() {
        auto map = KittyMemory::getMapsByName("libcocos2dcpp.so")[0];

        // fix cyrillic Р in comments
        patch(map, 0x657473, (uint8_t*)"\x01", 1);

        // change AchievementCell's width
        auto newAchWidth = 210.f;
        patch(map, 0x226F8C, (uint8_t*)&newAchWidth, 4);

        // change MapPackCell's View button height
        auto mapPackCellBtnHeight = 0.f; // 0 => auto detect height
        patch(map, 0x228E3C, (uint8_t*)&mapPackCellBtnHeight, 4);

        hook("_ZN19MultilineBitmapFont13readColorInfoESs", (void*)readColorInfo_hk, (void**)&readColorInfo_o);
        hook("_ZN19MultilineBitmapFont18stringWithMaxWidthESsff", (void*)stringWithMaxWidth_hk, (void**)&stringWithMaxWidth_o);
        hook("_ZN19MultilineBitmapFont12initWithFontEPKcSsffN7cocos2d7CCPointEib", (void*)initWithFont_hk, (void**)&initWithFont_o);
        hook("_ZN7cocos2d13CCApplication7openURLEPKc", (void*)openURL_hk, (void**)&openURL_o);
        hook("_ZN12GauntletNode4initEP9GJMapPack", (void*)GauntletNode_init_hk, (void**)&GauntletNode_init_o);
        hook("_ZN13GauntletLayer4initE12GauntletType", (void*)GauntletLayer_init_hk, (void**)&GauntletLayer_init_o);
        hook("_ZN16LevelLeaderboard4initEP11GJGameLevel20LevelLeaderboardType", (void*)LevelLeaderboard_init_hk,
             (void**)&LevelLeaderboard_init_o);
        hook("_ZN12OptionsLayer11customSetupEv", (void*)customSetup_hk, (void**)&customSetup_o);
        hook("_ZN7cocos2d13CCLabelBMFont8setScaleEf", (void*)CCLabelBMFont_setScale_hk, (void**)&CCLabelBMFont_setScale_o);
        hook("_ZN7cocos2d6CCNode11setPositionERKNS_7CCPointE", (void*)CCNode_setPos_hk, (void**)&CCNode_setPos_o);
        hook("_ZN9MenuLayer4initEv", (void*)MenuLayer_init_hk, (void**)&MenuLayer_init_o);
        hook("_ZN12LoadingLayer4initEb", (void*)LoadingLayer_init_hk, (void**)&LoadingLayer_init_o);

        // TODO: actually make it work
        // auto addr = map.startAddress + 0x734E74;
        // uintptr_t origAddr;
        // KittyMemory::memRead((void*)&origAddr, (void*)addr, 4);
        // origAddr -= 1;
        // CCLabelBMFont_setPosition_o = (decltype(CCLabelBMFont_setPosition_o))origAddr;
        // auto hkAddr = (uintptr_t)&CCLabelBMFont_setPosition_hk;
        // auto hkAddr = (uintptr_t)&CCLabelBMFont_setPosition_hk + 1;
        // auto hkAddr2 = (hkAddr & 0xFF) << 24 | (hkAddr & 0xFF00) << 8 | (hkAddr & 0xFF0000) >> 8 | (hkAddr & 0xFF000000) >> 24;
        // logD("before patch");
        // patch(addr, (uint8_t*)hkAddr2, 4);
        // logD("after patch");
        // patch(addr, (uint8_t*)hkAddr, 4);

        // logD("Orig addr {:X}, hk addr {:X} coocs base {:X} addr2 {:X}", origAddr, hkAddr, map.startAddress, hkAddr2);
    }
} // namespace Hooks