#pragma once

#include <array>

#include "../iinput_handler.h"
#include "scoundrel_boardmanager.h"
#include "../animation/animation_engine.h"

class Scoundrel : public IInputHandler
{
public:
    auto init(AnimationEngine* engine) -> void;
    auto update() -> void;

    auto board() -> ScoundrelBoard&;
    auto handleClick(double xpos, double ypos, bool isDragging,
            bool isDragStart) -> void override;
    auto handleDoubleClick(double xpos, double ypos) -> void override;
    auto handleNewGame() -> void override;

private:
    auto createBgCards() -> void;

    auto executeMove(CardClickedScoundrel& selected,
                     CardClickedScoundrel& dst) -> void;
    auto moveBackAndDeselectCard() -> void;

    GameState m_currentState;
    int m_life;
    bool m_runnedLastRoom;
    std::array<CardBg, CARD_BG_SIZE> m_cardBg;
    ScoundrelBoardManager m_boardManager;
    AnimationEngine* animationEngine;
};
