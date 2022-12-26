#pragma once

#include <vector>
#include <stack>
#include <array>
#include <span>
#include <functional>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/gtx/compatibility.hpp>

#include "timer.h"
#include "window.h"

struct Card
{
    int number;
    int suit;
    int offsetX;
    int offsetY;
    glm::vec3 selectionTint;
    glm::vec2 dragOffset;
    glm::vec3 pos;
};

typedef std::vector<Card*> CardStack;

struct CardSelection
{
    int y{};
    Card* card{};
    CardStack* stack{};
    glm::vec2 pos{};
};

class MovingAnimation
{
public:
    MovingAnimation(std::span<Card*> cards, glm::vec2 startPos, glm::vec2 dstPos, std::function<void()> onComplete = nullptr)
        : m_startPos(startPos), m_dstPos(dstPos), m_cards(cards), m_onCompleteCallback(onComplete)
    {
        m_len = glm::max(glm::length(m_dstPos - m_startPos), 0.00001f);
    }

    void update()
    {
        // TODO: check if can find better way to initialize the start time
        if (!m_hasStarted)
        {
            m_startTime = Timer::time;
            m_hasStarted = true;
        }

        float distCovered = (Timer::time - m_startTime) * m_speed;
        float delta = distCovered / m_len;
        glm::vec3 pos;
        if (delta >= 1.0f)
        {
            for (int i = 0; i < (int) m_cards.size(); i++)
            {
                pos = glm::vec3(m_dstPos, 0.0);
                pos.y -= i * 32;
                m_cards[i]->pos = pos;
                isDone = true;
            }
            if (m_onCompleteCallback != nullptr)
                m_onCompleteCallback();
        }
        else
        {
            for (int i = 0; i < (int) m_cards.size(); i++)
            {
                pos = glm::vec3(glm::lerp(m_startPos, m_dstPos, delta), 0.0001);
                pos.y -= i * 32;
                m_cards[i]->pos = pos;
            }
        }        
    }

    bool isDone{};

private:
    float m_speed = 4000;
    float m_startTime;
    float m_len;
    glm::vec2 m_startPos;
    glm::vec2 m_dstPos;
    std::span<Card*> m_cards;
    std::function<void()> m_onCompleteCallback;
    bool m_hasStarted{};
};

class DraggingAnimation
{
public:
    void start(std::span<Card*> cards)
    {
        m_cards = cards;
        glm::vec2 mousePos = glm::vec2(Window::xPos, Window::yPos);
        for (Card* c : cards)
        {
            c->dragOffset = mousePos - glm::vec2(c->pos);
        }
        isDone = false;
    }

    void update()
    {
        glm::vec2 mousePos = glm::vec2(Window::xPos, Window::yPos);
        for (Card* c : m_cards)
        {
            c->pos = glm::vec3(mousePos - c->dragOffset, 0.0001);
        }
    }

    void stop()
    {
        isDone = true;
    }

    bool isDone{true};

private:
    std::span<Card*> m_cards;
};

struct Board
{
    std::array<std::array<glm::vec2, 12>, 8> tableMap;
    std::array<glm::vec2, 4> openCellsMap;
    std::array<glm::vec2, 4> foundationsMap;

    std::array<CardStack, 8> table;
    std::array<CardStack, 4> openCells;
    std::array<CardStack, 4> foundations;

    Card* openCellsBg;
    Card* foundationsBg;

    std::vector<MovingAnimation> movingAnimation;
    DraggingAnimation draggingAnimation;
};

struct Move
{
    CardStack* srcStack;
    CardStack* dstStack;
    int cardQuantity;
    glm::vec2 srcPos;
    glm::vec2 dstPos;
};

class History
{
public:
    void recordMove(CardStack* src, CardStack* dst, int n, glm::vec2 srcPos = glm::vec2{0}, glm::vec2 dstPos = glm::vec2{0});
    void undo();
    void redo();
    bool isUndoStackEmpty() const;
    bool isRedoStackEmpty() const;
    Move getTopUndoMove() const;
    Move getTopRedoMove() const;

private:
    std::stack<Move> m_undoStack;
    std::stack<Move> m_redoStack;
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

private:
    void setBoardLayout();
    void createDeck();
    void createOpenCellsAndFoundations();
    void swap(int i, int j);
    void shuffle();
    void fillTable();
    void emptyTable();

    bool checkWin();
    bool checkSequence(CardStack& stack, int j);
    bool isComplete();
    void playWinAnimation();
    int getMaxCardsToMove(bool movingToEmptySpace);

    bool openCellsIsLegalMove(Card* card, int col);
    bool foundationsIsLegalMove(Card* card, int col);
    bool tableIsLegalMove(Card* card, int col);

    void handleClick(CardStack& stack, std::span<glm::vec2> dstAreaPos, int col, int index, bool(Freecell::*isLegalMove)(Card* card, int c), bool isDragStart);

    void select(CardStack* stack, int index, bool isDragStart);
    void deselect();

    int getIndexX(int n, double xPos);
    int getIndexY(int n, int col, double yPos);

    void winMoves(CardStack& src, std::span<CardStack> dst, int col, std::span<glm::vec2> dstAreaPos, bool(Freecell::*isLegalMove)(Card* card, int c));
    bool tryMoveFromTo(CardStack& src, std::span<CardStack> dst, int col, std::span<glm::vec2> dstAreaPos, bool(Freecell::*isLegalMove)(Card* card, int c));
    void moveCard(CardStack& src, CardStack& dst, int n);

    std::vector<Card> m_deck;
    History m_history;
    CardSelection m_cardSelected{};
    Board m_board{};
    bool winState{};
};
