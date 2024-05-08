#pragma once

#include "../game_board.h"
#include "../dealer.h"

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

struct FreecellBoard
{
    CardStack cards;
    std::span<CardBg> cardBgs;

    std::array<CardStack, FREECELL_TABLEAU_SIZE> tableau;
    std::array<CardStack, SPECIAL_AREAS_SIZE> openCells;
    std::array<CardStack, SPECIAL_AREAS_SIZE> foundations;
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

class FreecellBoardManager : BoardManager
{
public:
    auto createDeck() -> void override;
    auto shuffleDeck() -> void override;
    auto emptyTable() -> void override;
    auto fillTable() -> void override;
    auto updateCardList() -> void override;

    auto turnCardsUp() -> void;
    auto turnCardsDown() -> void;

    // tmp stuff
    auto board() -> FreecellBoard&;
    auto boardMap() -> FreecellBoardMap&;

private:
    Dealer m_dealer;
    FreecellBoard m_board;
    FreecellBoardMap m_boardMap;
};

