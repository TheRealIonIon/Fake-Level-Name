#include <Geode/Geode.hpp>
#include <Geode/modify/LevelCell.hpp>
#include <Geode/modify/PauseLayer.hpp>

using namespace geode::prelude;

static std::string getName(GJGameLevel* level) {
    auto id = numToString(level->m_levelID.value());
    if (!Mod::get()->hasSavedValue(id)) return level->m_levelName;
    else return Mod::get()->getSavedValue<std::string>(id);
}

class $modify(LevelCell) {
    void loadFromLevel(GJGameLevel* level) {
        auto name = level->m_levelName;
        level->m_levelName = getName(level);

        LevelCell::loadFromLevel(level);
        level->m_levelName = name;
    }
};

class $modify(PauseLayer) {
    void setupLabel() {
        auto node = this->getChildByID("level-name");
        auto label = typeinfo_cast<CCLabelBMFont*>(node);
        auto anchor = label->getAnchorPoint();
        auto pos = label->getPosition();
        auto color = label->getColor();

        auto name = getName(PlayLayer::get()->m_level);
        auto title = CCLabelBMFont::create(name.c_str(), "bigFont.fnt");

        title->setID("title-label"_spr);
        title->setAnchorPoint(anchor);
        title->setPosition(pos);
        title->setColor(color);
        
        this->addChild(title);
        label->setVisible(false);
    }

    void customSetup() {
        PauseLayer::customSetup();

        queueInMainThread([this] {
            setupLabel();
        });
    }
};

// Commented for now, scheduled removal in the next major update
// #include <Geode/modify/FLAlertLayer.hpp>
// class $modify(FLAlertLayer) {
//     void onEnter() {
// 		FLAlertLayer::onEnter();

// 		if (this->getID() == "cvolton.betterinfo/ExtendedLevelInfo") {
//             auto layer = CCScene::get()->getChildByID("LevelInfoLayer");
//             auto level = typeinfo_cast<LevelInfoLayer*>(layer)->m_level;

// 			auto node = this->getChildByIDRecursive("cvolton.betterinfo/level-name-button");
// 			auto label = node->getChildByType<CCLabelBMFont>(0);
// 			label->setString(getName(level).c_str());
// 		} else {
//             auto layer = CCScene::get()->getChildByID("LevelInfoLayer");
//             auto node  = this->getChildByIDRecursive("title");
//             if (!node) node = this->getChildByIDRecursive("title-label");

//             if (!node || !layer) return;

//             auto label = typeinfo_cast<CCLabelBMFont*>(node);
//             auto level = typeinfo_cast<LevelInfoLayer*>(layer)->m_level;

//             if (level && label->getString() == level->m_levelName) {
//                 label->setString(getName(level).c_str());
//             }
//         }
// 	}
// };