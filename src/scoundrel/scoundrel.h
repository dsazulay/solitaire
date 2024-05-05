#pragma once

#include <array>

#include "../card.h"
#include "../dealer.h"

constexpr const int CARD_BG_SIZE = 4;
constexpr const int ROOM_SIZE = 4;
constexpr const int WPN_MAX_SIZE = 10;

struct ScoundrelBoard
{
    CardStack dungeon;
    CardStack discard;
    std::array<Card*, ROOM_SIZE> room;
    CardStack weapon;
    std::array<Card*, CARD_BG_SIZE> cardsBg;

    CardStack cards{};

    auto updateCards() -> void
    {
        cards.clear();
        if (discard.size() > 0)
        {
            cards.push_back(discard.back());
        }
        for (auto card : room)
        {
            if (card != nullptr)
            {
                cards.push_back(card);
            }
        }
        for (auto card : weapon)
        {
            cards.push_back(card);
        }
    }
};

struct ScoundrelBoardMap
{
    glm::vec2 dungeon;
    glm::vec2 discard;
    std::array<glm::vec2, 4> room;
    std::array<glm::vec2, WPN_MAX_SIZE> weapon;
    glm::vec2 potion;
    glm::vec2 hands;

    auto setBoardLayout() -> void
    {
        dungeon = { 400.0, 560.0 };
        discard = { 790.0, 560.0 };
        for (int i = 0; i < 4; ++i)
        {
            room[i] = { 400.0 + i * 130.0, 400.0 };
        }

        for (int i = 0; i < WPN_MAX_SIZE; ++i)
        {
            weapon[i] = {400 + i * 30.0, 240.0 };
        }
        potion = { 660.0, 240.0 };
        hands = { 790.0, 240.0 };
    }
};

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
    std::array<Card, CARD_BG_SIZE> m_cardBg;
    ScoundrelBoard m_board;
    ScoundrelBoardMap m_boardMap;
};
