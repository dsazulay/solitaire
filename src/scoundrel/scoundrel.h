#pragma once

#include <array>

#include "../game_board.h"
#include "../dealer.h"

class Scoundrel
{
public:
    auto init() -> void;
    auto update() -> void;

    auto board() -> ScoundrelBoard&;
private:
    auto createBgCards() -> void;

    int m_life;
    Dealer m_dealer;
    std::array<CardBg, CARD_BG_SIZE> m_cardBg;
    ScoundrelBoard m_board;
    ScoundrelBoardMap m_boardMap;
};
