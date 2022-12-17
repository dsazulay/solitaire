#pragma once

#include <vector>
#include <stack>
#include <array>
#include <span>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/gtx/compatibility.hpp>

#include "timer.h"
#include "window.h"

struct Card
{
    unsigned int number;
    unsigned int suit;
    unsigned int offsetX;
    unsigned int offsetY;
    glm::vec3 selectionTint;
    glm::vec2 dragOffset;
    glm::vec3 pos;

};

struct CardSelection
{
    int x{};
    int y{};
    Card* card{};
    std::vector<Card*>* area{};
    glm::vec2 pos{};
};

class MovingAnimation
{
public:
    MovingAnimation(std::span<Card*> cards, glm::vec2 startPos, glm::vec2 dstPos) : m_startPos(startPos), m_dstPos(dstPos), m_cards(cards)
    {
        m_startTime = Timer::time;
        m_len = glm::length(m_dstPos - m_startPos);
    }

    void update()
    {
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
    glm::vec2 tableMap[8][12];
    glm::vec2 openCellsMap[4];
    glm::vec2 foundationMap[4];

    std::vector<Card*> table[8];
    std::vector<Card*> openCells[4];
    std::vector<Card*> foundations[4];

    Card* openCellsBg;
    Card* foundationsBg;

    std::vector<MovingAnimation> movingAnimation;
    DraggingAnimation draggingAnimation;
};

struct Move
{
    std::vector<Card*>* srcStack;
    std::vector<Card*>* dstStack;
    int cardQuantity;
};

class History
{
public:
    void recordMove(std::vector<Card*>* src, std::vector<Card*>* dst, int n);
    void undo();
    void redo();

private:
    Move executeMove(const Move& move);

    std::stack<Move> m_undoStack;
    std::stack<Move> m_redoStack;
};

class Freecell
{
public:
    void init();
    void processInput(double xPos, double yPos, bool isDraging, bool isDragStart);
    void processDoubleClick(double xPos, double yPos);
    Board& board();
    void undoMove();
    void redoMove();
    void update();

private:
    void setBoardLayout();
    void createDeck();
    void createOpenCellsAndFoundations();
    void swap(int i, int j);
    void shuffle();
    void fillTable();

    bool checkWin();
    bool checkSequence(int i, int j);
    int getMaxCardsToMove(bool movingToEmptySpace);

    bool openCellsIsLegalMove(Card* card, int col);
    bool foundationsIsLegalMove(Card* card, int col);
    bool tableIsLegalMove(Card* card, int col);

    void handleOpenCellsClick(int i, bool isDragStart);
    void handleFoundationsClick(int i, bool isDragStart);
    void handleTableClick(int i, int j, bool isDragStart);

    void select(std::vector<Card*>* area, int x, int y, bool isDragStart);
    void deselect();

    int getIndexX(int n, double xPos);
    int getIndexY(int n, int col, double yPos);

    bool tryMoveFromTo(std::vector<Card*>& src, std::span<std::vector<Card*>> dst, int col, std::span<glm::vec2> dstAreaPos, bool(Freecell::*isLegalMove)(Card* card, int c));
    void moveCard(std::vector<Card*>& src, std::vector<Card*>& dst, int n);

    std::vector<Card> m_deck;

    History m_history;

    CardSelection m_cardSelected{};
    Board m_board{};
};
