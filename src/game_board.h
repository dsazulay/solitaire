#pragma once

#include <span>
#include <array>
#include <glm/vec2.hpp>
#include "card.h"

// freecell consts
constexpr const int FREECELL_TABLEAU_SIZE = 8;
constexpr const int FREECELL_MAX_STACK_SIZE = 14;
constexpr const int SPECIAL_AREAS_SIZE = 4;
constexpr const float SPECIAL_AREAS_Y = 560.0;
constexpr const float TABLEAU_INIT_POS_X = 220.0f;
constexpr const float TABLEAU_OFFSET_X = 120.0f;
constexpr const float TABLEAU_INIT_POS_Y = 380.0f;
constexpr const float TABLEAU_OFFSET_Y = 30.0f;
constexpr const float OPENCELLS_INIT_POS_X = 140.0f;
constexpr const float SPECIAL_AREAS_OFFSET_X = 130.0f;
constexpr const float FOUNDATIONS_INIT_POS_X = OPENCELLS_INIT_POS_X +
    90.f + 4 * SPECIAL_AREAS_OFFSET_X;
constexpr const float CARD_HALF_WIDTH = 50.0f;
constexpr const float CARD_HALF_HEIGHT = 74.0f;
constexpr const float CARD_MIDDLE_HEIGHT = 42.0f;
constexpr const float HALF_SCREEN_WIDTH = 640.0f;


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

struct Board
{
    virtual ~Board() = default;
    virtual auto updateCardList() -> void = 0;

    CardStack cards;
    std::span<CardBg> cardBgs;
};

struct FreecellBoard : public Board
{
    std::array<CardStack, FREECELL_TABLEAU_SIZE> tableau;
    std::array<CardStack, SPECIAL_AREAS_SIZE> openCells;
    std::array<CardStack, SPECIAL_AREAS_SIZE> foundations;

    auto updateCardList() -> void override
    {
        cards.clear();
        for (auto stack : tableau)
        {
            for (auto card : stack)
            {
                cards.push_back(card);
            }
        }

        for (auto stack : openCells)
        {
            if (stack.size() > 0)
            {
                cards.push_back(stack.back());
            }
        }

        for (auto stack : foundations)
        {
            unsigned long size = stack.size();
            if (size > 0)
            {
                if (size > 1)
                {
                    cards.push_back(stack[size - 2]);
                }
                cards.push_back(stack.back());
            }
        }

    }
};

struct FreecellBoardMap
{

    std::array<float, FREECELL_TABLEAU_SIZE> tableauX{};
    std::array<float, FREECELL_MAX_STACK_SIZE> tableauY{};
    std::array<float, SPECIAL_AREAS_SIZE> openCells{};
    std::array<float, SPECIAL_AREAS_SIZE> foundations{};

    FreecellBoardMap()
    {
        for (int i = 0; i < FREECELL_TABLEAU_SIZE; i++)
        {
            tableauX[i] = TABLEAU_INIT_POS_X + (float) i * TABLEAU_OFFSET_X;
        }

        for (int i = 0; i < FREECELL_MAX_STACK_SIZE; i++)
        {
            tableauY[i] = TABLEAU_INIT_POS_Y - (float) i * TABLEAU_OFFSET_Y;
        }


        for (int i = 0; i < SPECIAL_AREAS_SIZE; i++)
        {
            auto offset = (float) i * SPECIAL_AREAS_OFFSET_X;
            openCells[i] = OPENCELLS_INIT_POS_X + offset;
            foundations[i] = FOUNDATIONS_INIT_POS_X + offset;
        }
    }
};

struct ScoundrelBoard : public Board
{
    CardStack dungeon;
    CardStack discard;
    std::array<CardEntity*, ROOM_SIZE> room;
    CardStack weapon;

    auto updateCardList() -> void override
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

