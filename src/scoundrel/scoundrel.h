#pragma once

#include <array>

#include "../game_board.h"
#include "../iinput_handler.h"
#include "scoundrel_boardmanager.h"

class Scoundrel : public IInputHandler
{
public:
    auto init() -> void;
    auto update() -> void;

    auto board() -> ScoundrelBoard&;
    auto handleClick(double xpos, double ypos, bool isDragging,
            bool isDragStart) -> void override;
    auto handleDoubleClick(double xpos, double ypos) -> void override;
    auto handleNewGame() -> void override;

private:
    auto createBgCards() -> void;

    int m_life;
    std::array<CardBg, CARD_BG_SIZE> m_cardBg;
    ScoundrelBoardManager m_boardManager;
};
