#include "menu.hpp"

GDLMenu::GDLMenu() : m_page1(nullptr), m_page2(nullptr), m_page(0), m_nextBtn(nullptr), m_prevBtn(nullptr) {}

void GDLMenu::switchToPage(int page) {
    m_page = page;

    switch (page) {
    case 1:
        m_prevBtn->setEnabled(false);
        m_prevBtn->setVisible(false);

        m_nextBtn->setEnabled(true);
        m_nextBtn->setVisible(true);

        m_page1->setVisible(true);
        m_page2->setVisible(false);
        break;
    case 2:
        m_prevBtn->setEnabled(true);
        m_prevBtn->setVisible(true);

        m_nextBtn->setEnabled(false);
        m_nextBtn->setVisible(false);

        m_page1->setVisible(false);
        m_page2->setVisible(true);
        break;
    }
}

void GDLMenu::changePage(CCObject* pObj) {
    switchToPage(m_page + (pObj->getTag() ? 1 : -1));
}

void GDLMenu::openLink(CCObject*) {
    CCApplication::sharedApplication()->openURL("https://bit.ly/3K4SbF2");
}

void GDLMenu::setup() {
    m_page1 = CCLayer::create();
    m_page2 = CCLayer::create();

    // title

    auto title = CCSprite::createWithSpriteFrameName("gdl_logo_text.png");
    title->setScale(1.1f);
    auto titlespr = CCMenuItemSprite::create(title, title, this, (SEL_MenuHandler)&GDLMenu::openLink);
    titlespr->runAction(CCRepeatForever::create(CCSequence::create(CCEaseInOut::create(CCScaleTo::create(1.5f, 1.2f), 2.0f),
                                                                   CCEaseInOut::create(CCScaleTo::create(1.5f, 1.1f), 2.0f), nullptr)));

    titlespr->setPosition({-10, 84});
    m_buttonMenu->addChild(titlespr);

    // // page 1

    auto a1 = CCLabelBMFont::create("Разработчики:", "goldFont.fnt");
    m_page1->addChild(a1);
    a1->setScale(0.8f);
    a1->setPositionY(61);

    MENU_PROFILE_MAKE("Jaan", "jaan.png", jaan_pr, false)
    MENU_PROFILE_MAKE("Mye", "mye.png", mye_pr, false)
    MENU_PROFILE_MAKE("Demi Sans", "demisans.png", demi_pr, true)
    MENU_PROFILE_MAKE("RayChan", "lukasradon.png", lukas_pr, true)
    MENU_PROFILE_MAKE("RNMNX", "guschin.png", gus_pr, false)

    auto row1 = CCMenu::create(jaan_pr, mye_pr, demi_pr, lukas_pr, gus_pr, nullptr);
    m_page1->addChild(row1);
    row1->alignItemsHorizontallyWithPadding(55);
    row1->setPosition({0, 26});

    MENU_PROFILE_MAKE("Махимал", "maximal.png", max_pr, false)
    MENU_PROFILE_MAKE("Raelc", "raelc.png", raelc_pr, false)
    MENU_PROFILE_MAKE("Pixelsuft", "noname.png", noname_pr, false)
    MENU_PROFILE_MAKE("Astragel", "uulti.png", uulti_pr, false)

    auto row2 = CCMenu::create(max_pr, raelc_pr, noname_pr, uulti_pr, nullptr);
    m_page1->addChild(row2);
    row2->alignItemsHorizontallyWithPadding(55);
    row2->setPosition({0, -30});

    m_mainLayer->addChild(m_page1);
    m_page1->setPosition({CCDirector::sharedDirector()->getWinSize().width / 2, 157});

    // // page 2

    auto a2 = CCLabelBMFont::create("Отдельная благодарность:", "goldFont.fnt");
    m_page2->addChild(a2);
    a2->setScale(0.7f);
    a2->setPositionY(63);

    const char* thanks1 = "- RobTop\n"
                          "- QuasarKim\n"
                          "- Kolyah35\n"
                          "- Geode SDK\n"
                          "- GD Programming\n"
                          "- Andre\n"
                          "- mat\n"
                          "- HJFod\n"
                          "- Ayrelia";

    const char* thanks2 = "- alk1m123\n"
                          "- Cvolton\n"
                          "- Wylie\n"
                          "- sleepyut\n"
                          "- Log0\n"
                          "- camila314\n"
                          "- Eldyj\n"
                          "- Rektor\n"
                          "- Тестерам GDL";

    auto ta1 = CCLabelBMFont::create(thanks1, "goldFont.fnt");
    ta1->setAnchorPoint({0,1});
    ta1->setScale(0.6f);
    ta1->setPosition({-130, 50});
    auto ta2 = CCLabelBMFont::create(thanks2, "goldFont.fnt");
    ta2->setAnchorPoint({0,1});
    ta2->setScale(0.6f);
    ta2->setPosition({40, 50});
    m_page2->addChild(ta1);
    m_page2->addChild(ta2);

    m_page2->setPosition({CCDirector::sharedDirector()->getWinSize().width / 2, 157});
    m_mainLayer->addChild(m_page2);

    // btns

    auto _spr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    _spr->setFlipX(true);
    m_nextBtn = CCMenuItemSpriteExtra::create(_spr, _spr, this, (SEL_MenuHandler)&GDLMenu::changePage);
    m_nextBtn->setTag(1);
    m_nextBtn->setPositionX(225);
    m_buttonMenu->addChild(m_nextBtn);

    auto _spr2 = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    m_prevBtn = CCMenuItemSpriteExtra::create(_spr2, _spr2, this, (SEL_MenuHandler)&GDLMenu::changePage);
    m_prevBtn->setTag(0);
    m_prevBtn->setPositionX(-225);
    m_buttonMenu->addChild(m_prevBtn);

    // close btn

    auto closeBtnSpr = ButtonSprite::create("OK");
    auto closeBtn = CCMenuItemSpriteExtra::create(dynamic_cast<CCSprite*>(closeBtnSpr), dynamic_cast<CCSprite*>(closeBtnSpr), this,
                                                  (SEL_MenuHandler)&GDLMenu::onClose);
    closeBtn->setUserObject(this);
    closeBtn->setPositionY(-95);
    m_buttonMenu->addChild(closeBtn);

    switchToPage(1);
}

void GDLMenu::keyBackClicked() {
    onClose(nullptr);
}

void GDLMenu::openLayer(CCObject* pObj) {
    auto scene = CCDirector::sharedDirector()->getRunningScene();
    scene->addChild(GDLMenu::create(), scene->getHighestChildZ() + 1);
}

GDLMenu* GDLMenu::create() {
    auto pRet = new GDLMenu();

    if (pRet && pRet->init(400, 230)) {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}