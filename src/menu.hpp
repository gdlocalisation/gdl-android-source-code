#pragma once
#include "includes.hpp"
#include "BrownAlertDelegate.hpp"

#define MENU_PROFILE_MAKE(name, tex, var, smallNick)                                                                                       \
    auto var = CCMenuItem::create();                                                                                                       \
    var->addChild(CCSprite::createWithSpriteFrameName(tex));                                                                               \
    auto text_profile_##var = CCLabelBMFont::create(name, "goldFont.fnt");                                                                 \
    text_profile_##var->setPositionY((smallNick) ? -23.5f : -23.0f);                                                                       \
    text_profile_##var->limitLabelWidth(30.f, 0.5f, 0.35f);                                                                                \
    var->addChild(text_profile_##var);

class GDLMenu : public BrownAlertDelegate {
  private:
    CCLayer* m_page1;
    CCLayer* m_page2;
    int m_page;
    CCMenuItemSpriteExtra* m_nextBtn;
    CCMenuItemSpriteExtra* m_prevBtn;

    GDLMenu();
    void switchToPage(int page);
    void keyBackClicked() override;
    virtual void setup() override;
    void openLink(CCObject*);
    void changePage(CCObject* pObj);

  public:
    static GDLMenu* create();
    void openLayer(CCObject*);
};