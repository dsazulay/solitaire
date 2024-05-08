#pragma once

#include <span>
#include <array>
#include <glm/vec2.hpp>
#include "card.h"

// scoundrell consts
constexpr const int CARD_BG_SIZE = 4;
constexpr const int ROOM_SIZE = 4;
constexpr const int WPN_MAX_SIZE = 10;
constexpr const glm::vec2 DUNGEON_POS{ 400.0, 560.0 };
constexpr const glm::vec2 DISCARD_POS{ 790.0, 560.0 };
constexpr const glm::vec2 POTION_POS{ 660.0, 240.0 };
constexpr const glm::vec2 HANDS_POS{ 790.0, 240.0 };
constexpr const glm::vec2 ROOM_POS{ 400.0, 400.0 };
constexpr const glm::vec2 WPN_POS{ 400.0, 240.0 };
constexpr const float ROOM_OFFSET = 130.0;
constexpr const float WPN_OFFSET = 30.0;

class BoardManager
{
public:
    virtual ~BoardManager() = default;
    virtual auto createDeck() -> void = 0;
    virtual auto shuffleDeck() -> void = 0;
    virtual auto emptyTable() -> void = 0;
    virtual auto fillTable() -> void = 0;
    virtual auto updateCardList() -> void = 0;
};

struct ScoundrelBoard
{
    CardStack cards;
    std::span<CardBg> cardBgs;
    CardStack dungeon;
    CardStack discard;
    std::array<CardEntity*, ROOM_SIZE> room;
    CardStack weapon;

    auto updateCardList()
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
    glm::vec2 potion;
    glm::vec2 hands;
    std::array<glm::vec2, ROOM_SIZE> room{};
    std::array<glm::vec2, WPN_MAX_SIZE> weapon{};

    // move this to struct constructor
    ScoundrelBoardMap() : dungeon{ DUNGEON_POS }, discard{ DISCARD_POS },
        potion{ POTION_POS }, hands{ HANDS_POS }
    {
        for (int i = 0; i < ROOM_SIZE; ++i)
        {
            room[i] = { ROOM_POS.x + (float) i * ROOM_OFFSET, ROOM_POS.y };
        }

        for (int i = 0; i < WPN_MAX_SIZE; ++i)
        {
            weapon[i] = { WPN_POS.x + (float) i * WPN_OFFSET, WPN_POS.y };
        }
    }
};

