#pragma once

#include <vector>
#include <stack>
#include <array>
#include <span>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/gtx/compatibility.hpp>

#include "timer.h"

struct Card
{
    unsigned int number;
    unsigned int suit;
    unsigned int offsetX;
    unsigned int offsetY;
    glm::vec3 selectionTint;
    bool dragging{};
    bool shouldSetOffset{};
    glm::vec2 dragOffset;
    bool isMoving{};
    glm::vec2 pos;

};

struct Transform
{
    glm::vec2 pos;
    glm::vec2 scale;
};

struct CardActor
{
    Card card;
    Transform transform;
    glm::vec2 dragOffset;
};

struct CardSelection
{
    int x{};
    int y{};
    Card* card{};
    std::vector<Card*>* area{};
};

class MovingAnimation
{
public:
    MovingAnimation(Card* card, glm::vec2 startPos, glm::vec2 dstPos) : m_startPos(startPos), m_dstPos(dstPos), m_card(card)
    {
        m_startTime = Timer::time;
        m_len = glm::length(m_dstPos - m_startPos);
        m_card->isMoving = true;
    }

    void update()
    {
        float distCovered = (Timer::time - m_startTime) * m_speed;
        float delta = distCovered / m_len;
        glm::vec2 pos;
        if (delta >= 1.0f)
        {
            pos = m_dstPos;
            isDone = true;
            m_card->isMoving = false;
        }
        else
            pos = glm::lerp(m_startPos, m_dstPos, delta);
        
        m_card->pos = pos;
    }

    bool isDone{};

private:
    float m_speed = 4000;
    float m_startTime;
    float m_len;
    glm::vec2 m_startPos;
    glm::vec2 m_dstPos;
    Card* m_card;
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
    void deselectTrailingCards();

    bool moveCardToFoundations(std::vector<Card*>& src, int col);
    bool moveCardToOpenCells(std::vector<Card*>& src, int col);

    int getIndexX(int n, double xPos);
    int getIndexY(int n, int col, double yPos);

    void moveCard(std::vector<Card*>& src, std::vector<Card*>& dst, int n);

    std::vector<Card> m_deck;

    History m_history;

    CardSelection m_cardSelected{};
    Board m_board{};
};
