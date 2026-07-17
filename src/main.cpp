#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

#define hasValue Mod::get()->hasSavedValue
#define getValue Mod::get()->getSavedValue<std::string>
#define eraseValue Mod::get()->getSaveContainer().erase

using namespace geode::prelude;

class CustomPopup : public Popup {
protected:
    TextInput* m_inputNode = nullptr;
    std::string m_defaultValue = "";
	std::string m_levelID = "";

    bool init(GJGameLevel* level) {
        if (!Popup::init(320.f, 150.f, "square01_001.png")) return false;

        this->setID("EditLevelNamePopup");
        m_defaultValue = level->m_levelName;
        m_levelID = numToString(level->m_levelID.value());
        
        this->setTitle("Edit Level Name", "goldFont.fnt", 1.f, 25.f);

        auto trashBtn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_trashBtn_001.png"),
            this, menu_selector(CustomPopup::onClear)
        );

        trashBtn->setScale(0.8f);
        trashBtn->m_baseScale = 0.8f;

        trashBtn->setPosition({ 45.f, m_size.height / 2 });
        m_buttonMenu->addChild(trashBtn);

        auto initialValue = hasValue(m_levelID) ? getValue(m_levelID) : m_defaultValue;
        m_inputNode = TextInput::create(170.f, "Enter level name...");
        m_inputNode->setPosition({ m_size.width / 2, m_size.height / 2 });
        
        m_inputNode->setMaxCharCount(20);
        m_inputNode->setString(initialValue.c_str());
        m_mainLayer->addChild(m_inputNode, 1);

        auto resetBtn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png"),
            this, menu_selector(CustomPopup::onReset)
        );

        resetBtn->setScale(0.75f);
        resetBtn->m_baseScale = 0.75f;

        resetBtn->setPosition({ m_size.width - 45.f, m_size.height / 2 });
        m_buttonMenu->addChild(resetBtn);

        auto cancelBtn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("Cancel", "goldFont.fnt", "GJ_button_06.png", 0.8f),
            this, menu_selector(CustomPopup::onCancel)
        );

        cancelBtn->setPosition({ (m_size.width / 2) - 35.f, 30.f });
        m_buttonMenu->addChild(cancelBtn);

        auto confirmBtn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("OK", "goldFont.fnt", "GJ_button_01.png", 0.8f),
            this, menu_selector(CustomPopup::onConfirm)
        );

        confirmBtn->setPosition({ (m_size.width / 2) + 55.f, 30.f });
        m_buttonMenu->addChild(confirmBtn);

        m_closeBtn->setVisible(false);
        m_closeBtn->setEnabled(false);

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