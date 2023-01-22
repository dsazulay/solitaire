#pragma once

#include <vector>
#include <stack>
#include <array>
#include <span>
#include <functional>

#include <glm/vec2.hpp>

#include "timer.h"
#include "window.h"
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
    Won
};

struct Board
{
    std::array<CardStack, 8> tableau;
    std::array<CardStack, 4> openCells;
    std::array<CardStack, 4> foundations;

    std::array<Card*, 4> openCellsBg;
    std::array<Card*, 4> foundationsBg;
};

struct BoardMap
{
    std::array<std::array<glm::vec2, 12>, 8> tableau;
    std::array<glm::vec2, 4> openCells;
    std::array<glm::vec2, 4> foundations;
};

struct Move
{
    CardStack* srcStack;
    CardStack* dstStack;
    int cardQuantity;
    glm::vec2 srcPos;
    glm::vec2 dstPos;

    Move swap()
    {
        Move newMove{dstStack, srcStack, cardQuantity, dstPos, srcPos};
        return newMove;
    }
};

class Freecell
{
public:
    void init();
    void update();

    void handleInputClick(double xPos, double yPos, bool isDraging, bool isDragStart);
    void handleInputDoubleClick(double xPos, double yPos);
    void handleInputUndo();
    void handleInputRedo();
    void handleInputRestart();
    void handleInputNewGame();

    Board& board();
    PlayerData* playerData();
    MatchData* matchData();

private:
    void loadPlayerData();
    void updatePlayerData(bool didWon, float time);

    void setBoardLayout();
    void createOpenCellsAndFoundations();

    bool checkWin();
    bool checkSequence(CardStack& stack, int j);
    bool checkWinSequence(CardStack& stack);
    bool isComplete();
    void playWinAnimation();
    int getMaxCardsToMove(bool movingToEmptySpace);

    bool openCellsIsLegalMove(Card* card, int col);
    bool foundationsIsLegalMove(Card* card, int col);
    bool tableIsLegalMove(Card* card, int col);

    void handleClick(CardStack& stack, std::span<glm::vec2> dstAreaPos, int col, int index, bool(Freecell::*isLegalMove)(Card* card, int c), bool isDragStart);

    void select(CardStack* stack, int index, bool isDragStart);
    void deselect();
    void moveBackAndDeselectCard();

    int getIndexX(int n, double xPos);
    int getTopAreaIndexX(std::span<glm::vec2> area, double xPos);
    int getIndexY(int n, int col, double yPos);

    void winMoves(CardStack& src, std::span<CardStack> dst, std::span<glm::vec2> dstAreaPos, bool(Freecell::*isLegalMove)(Card* card, int c));
    bool tryMoveFromTo(CardStack& src, std::span<CardStack> dst, std::span<glm::vec2> dstAreaPos, bool(Freecell::*isLegalMove)(Card* card, int c));
    void moveCard(CardStack& src, CardStack& dst, int n);

    History<Move> m_history;
    std::vector<MovingAnimation> m_movingAnimation;
    DraggingAnimation m_draggingAnimation;

    GameState m_currentState;
    PlayerData m_playerData;
    MatchData m_matchData;

    BoardMap m_boardMap{};
    CardSelection m_cardSelected{};

    Dealer m_dealer;
    std::array<Card, 4> m_openCellsBg;
    std::array<Card, 4> m_foundationsBg;
    Board m_board{};
};
