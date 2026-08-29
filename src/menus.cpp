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
    void customSetup() {
        PauseLayer::customSetup();
        auto level = PlayLayer::get()->m_level;

        if (level->m_levelType != GJLevelType::Editor) {
            if (auto title = this->getChildByID("level-name")) {
                auto label = typeinfo_cast<CCLabelBMFont*>(title);
                if (label) label->setString(getName(level).c_str());
            }
        }
    }
};