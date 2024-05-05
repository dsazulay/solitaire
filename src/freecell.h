#pragma once

#include <vector>
#include <array>
#include <span>

#include <glm/vec2.hpp>

#include "history.h"
#include "gamedata.h"
#include "card.h"
#include "dealer.h"
#include "dragging_animation.h"
#include "moving_animation.h"

struct CardSelection
{
    int y{};
    Card* card{};
    CardStack* stack{};
    glm::vec2 pos{};
};

enum class GameState
{
    Playing,
    WinAnimation,
    Won,
    Pause
};

struct Board
{
    constexpr static int tableauSize = 8;
    constexpr static int openCellsAndFoundSize = 4;
    constexpr static int stackMaxSize = 14;

    std::array<CardStack, tableauSize> tableau;
    std::array<CardStack, openCellsAndFoundSize> openCells;
    std::array<CardStack, openCellsAndFoundSize> foundations;

    std::array<Card*, openCellsAndFoundSize * 2> openCellsAndFoundBg{};

    std::vector<Card*> cards{};

    auto updateCards() -> void
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

struct BoardMap
{
    constexpr static float cardHalfWidth = 50.0f;
    constexpr static float cardHalfHeight = 74.0f;
    constexpr static float cardMiddleHeight = 42.0f;
    constexpr static float topAreaYPos = 560.0f;
    constexpr static float halfScreenWidth = 640.0f;

    std::array<float, Board::tableauSize> tableauX;
    std::array<float, Board::stackMaxSize> tableauY;
    std::array<float, Board::openCellsAndFoundSize> openCells;
    std::array<float, Board::openCellsAndFoundSize> foundations;
};

struct Move
{
    CardStack* srcStack;
    CardStack* dstStack;
    int cardQuantity;
    glm::vec2 srcPos;
    glm::vec2 dstPos;

    auto swap() -> Move
    {
        Move newMove{dstStack, srcStack, cardQuantity, dstPos, srcPos};
        return newMove;
    }
};

class Freecell
{
    using IsLegalMoveFunc = bool(Freecell::*)(Card* card, const CardStack& stack);
public:
    auto  init() -> void;
    auto  update() -> void;

    auto handleInputClick(double xPos, double yPos, bool isDraging, bool isDragStart) -> void;
    auto handleInputDoubleClick(double xPos, double yPos) -> void;
    auto handleInputUndo() -> void;
    auto handleInputRedo() -> void;
    auto handleInputRestart() -> void;
    auto handleInputNewGame() -> void;
    auto handleInputPause() -> void;
    auto handleInputPrintCards() -> void;

    auto board() -> Board&;
    auto playerData() -> PlayerData*;
    auto matchData() -> MatchData*;

private:
    auto loadPlayerData() -> void;
    auto updatePlayerData(bool didWon, float time) -> void;

    auto setBoardLayout() -> void;
    auto createOpenCellsAndFoundations() -> void;

    auto checkWin() -> bool;
    auto checkSequence(const CardStack& stack, int j) -> bool;
    auto checkWinSequence(const CardStack& stack) -> bool;
    auto isComplete() -> bool;
    auto playWinAnimation() -> void;
    auto getMaxCardsToMove(bool movingToEmptySpace) -> int;

    auto openCellsIsLegalMove(Card* card, const CardStack& stack) -> bool;
    auto foundationsIsLegalMove(Card* card, const CardStack& stack) -> bool;
    auto tableIsLegalMove(Card* card, const CardStack& stack) -> bool;

    auto handleClick(CardStack& stack, glm::vec2 dstPos, int col, int index, IsLegalMoveFunc isLegalMove, bool isDragStart) -> void;

    auto select(CardStack* stack, int index, bool isDragStart) -> void;
    auto deselect() -> void;
    auto moveBackAndDeselectCard() -> void;

    auto getIndexX(std::span<float> area, double xPos) -> int;
    auto getIndexY(int stackSize, double yPos) -> int;

    auto winMoves(CardStack& src, std::span<CardStack> dst, std::span<float> dstAreaPos, IsLegalMoveFunc isLegalMove) -> void;
    auto tryMoveFromTo(CardStack& src, std::span<CardStack> dst, std::span<float> dstAreaPos, IsLegalMoveFunc isLegalMove) -> bool;
    auto moveCard(CardStack& src, CardStack& dst, int n) -> void;

    History<Move> m_history;
    std::vector<MovingAnimation> m_movingAnimation;
    DraggingAnimation m_draggingAnimation;

    GameState m_currentState{};
    PlayerData m_playerData{};
    MatchData m_matchData{};

    BoardMap m_boardMap{};
    CardSelection m_cardSelected{};

    Dealer m_dealer;
    std::array<Card, 4> m_openCellsBg;
    std::array<Card, 4> m_foundationsBg;
    Board m_board{};
};
