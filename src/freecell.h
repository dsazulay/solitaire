#pragma once

#include <vector>
#include <stack>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

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
};


struct CardSelection
{
    int x{};
    int y{};
    Card* card{};
    std::vector<Card*>* area{};
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
};

struct Move
{
    std::vector<Card*>* srcStack;
    std::vector<Card*>* dstStack;
    int cardQuantity;
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

    bool isLegalMoveTable(std::vector<Card*>* stack, int srcX, int srcY, int dst);
    bool isLegalMoveFoundation(Card* card, int dst);

    void handleOpenCellsClick(int i, bool isDragStart);
    void handleFoundationsClick(int i, bool isDragStart);
    void handleTableClick(int i, int j, bool isDragStart);

    void select(std::vector<Card*>* area, int x, int y, bool isDragStart);
    void deselect();
    void deselectTrailingCards();

    bool moveCardToFoundations(std::vector<Card*>& src);
    bool moveCardToOpenCells(std::vector<Card*>& src);

    int getIndexX(int n, double xPos);
    int getIndexY(int n, int col, double yPos);

    std::vector<Card> m_deck;

    std::stack<Move> m_undoStack;
    std::stack<Move> m_redoStack;

    CardSelection m_cardSelected{};
    Board m_board{};
};
