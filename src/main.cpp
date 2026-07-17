#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

#define hasValue Mod::get()->hasSavedValue
#define getValue Mod::get()->getSavedValue<std::string>
#define eraseValue Mod::get()->getSaveContainer().erase

using namespace geode::prelude;

class CustomPopup : public FLAlertLayer {
protected:
    TextInput* m_inputNode = nullptr;
    std::string m_defaultValue = "";
	std::string m_levelID = "";

    bool init(GJGameLevel* level) {
        if (!FLAlertLayer::init(150)) return false;

        this->setID("EditLevelNamePopup");
		m_defaultValue = level->m_levelName;
		m_levelID = numToString(level->m_levelID.value());

		auto initialValue = hasValue(m_levelID) ? getValue(m_levelID) : m_defaultValue;
        CCPoint center = CCDirector::sharedDirector()->getWinSize() / 2;

        auto title = CCLabelBMFont::create("Edit Level Name", "goldFont.fnt");
        title->setPosition(center + CCPoint{ 0.f, 52.5f });
        m_mainLayer->addChild(title, 1);

        auto bg = CCScale9Sprite::create("square01_001.png");
        bg->setContentSize({ 320.f, 150.f });
        bg->setPosition(center);
        m_mainLayer->addChild(bg);

        if (!m_buttonMenu) {
            m_buttonMenu = CCMenu::create();
            m_mainLayer->addChild(m_buttonMenu);
        } m_buttonMenu->setPosition({ 0, 0 });

        auto trashBtn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_trashBtn_001.png"),
            this, menu_selector(CustomPopup::onClear)
        );

        trashBtn->setScale(0.8f);
        trashBtn->m_baseScale = 0.8f;

        trashBtn->setPosition(center + CCPoint{ -115.f, 5.f });
        m_buttonMenu->addChild(trashBtn);

        m_inputNode = TextInput::create(175.f, "Enter level name...");
        m_inputNode->setPosition(center + CCPoint{ 0.f, 5.f });
        m_inputNode->setString(initialValue.c_str());
        m_inputNode->setMaxCharCount(20);
        m_mainLayer->addChild(m_inputNode, 1);

        auto resetBtn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png"),
            this, menu_selector(CustomPopup::onReset)
        );

        resetBtn->setPosition(center + CCPoint{ 115.f, 5.f });
        m_buttonMenu->addChild(resetBtn);

        resetBtn->setScale(0.75f);
        resetBtn->m_baseScale = 0.75f;

        auto cancelBtn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("Cancel", "goldFont.fnt", "GJ_button_06.png", 0.8f),
            this, menu_selector(CustomPopup::onCancel)
        );
        
        cancelBtn->setPosition(center + CCPoint{ -40.5f, -45.f });
        m_buttonMenu->addChild(cancelBtn);

        auto confirmBtn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("OK", "goldFont.fnt", "GJ_button_01.png", 0.8f),
            this, menu_selector(CustomPopup::onConfirm)
        );

        confirmBtn->setPosition(center + CCPoint{ 60.f, -45.f });
        m_buttonMenu->addChild(confirmBtn);

        this->setTouchEnabled(true);
        this->setKeypadEnabled(true);

        return true;
    }

    void onClear(CCObject* sender) {
        if (m_inputNode) m_inputNode->setString("");
    }

    void onReset(CCObject* sender) {
        if (m_inputNode) m_inputNode->setString(m_defaultValue.c_str());
    }

    void onConfirm(CCObject* sender) {
        std::string finalValue = m_inputNode ? m_inputNode->getString() : "";

        if (finalValue.empty() || finalValue == m_defaultValue) eraseValue(m_levelID); 
        else Mod::get()->setSavedValue(m_levelID, finalValue);

        auto infoLayer = CCScene::get()->getChildByID("LevelInfoLayer");
        auto titleNode = infoLayer->getChildByIDRecursive("title-label"_spr);
        auto titleLabel = typeinfo_cast<CCLabelBMFont*>(titleNode);
        auto newText = hasValue(m_levelID) ? getValue(m_levelID) : m_defaultValue;
        
        titleLabel->setString(newText.c_str());
        titleNode->getParent()->updateLayout();
        
        this->keyBackClicked();
    }

    void onCancel(CCObject* sender) {
        this->keyBackClicked();
    }

public:
    static CustomPopup* create(GJGameLevel* level) {
        auto popup = new CustomPopup();

        if (popup && popup->init(level)) {
            popup->autorelease();
            return popup;
        }

        CC_SAFE_DELETE(popup);
        return nullptr;
    }
};

class $modify(MyLevelInfoLayer, LevelInfoLayer) {
	bool init(GJGameLevel* level, bool challenge) {
		if (!LevelInfoLayer::init(level, challenge)) return false;

		auto myButton = CCMenuItemSpriteExtra::create(
			CircleButtonSprite::createWithSprite("pencil.png"_spr, 0.9f),
			this, menu_selector(MyLevelInfoLayer::onButton)
		);

        auto leftMenu = this->getChildByID("left-side-menu");
        leftMenu->addChild(myButton);
        leftMenu->updateLayout();

        auto titleLabel = this->getChildByID("title-label");
        auto idStr = numToString(level->m_levelID.value());
        titleLabel->setVisible(false);

        auto newText = hasValue(idStr) ? getValue(idStr) : static_cast<std::string>(level->m_levelName);
        auto newLabel = CCLabelBMFont::create(newText.c_str(), "bigFont.fnt");
        newLabel->setColor(typeinfo_cast<CCLabelBMFont*>(titleLabel)->getColor());
        
        auto titleMenu = CCMenu::create();
        titleMenu->setPosition({0, 0});
        this->addChild(titleMenu);

        newLabel->setAnchorPoint(titleLabel->getAnchorPoint());
        newLabel->setPosition(titleLabel->getPosition());
        newLabel->setScale(titleLabel->getScale());
        newLabel->setID("title-label"_spr);
        titleMenu->addChild(newLabel);
 
		return true;
	}

    void onButton(CCObject* sender) {
		CustomPopup::create(m_level)->show();
	}
};