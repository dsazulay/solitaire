#pragma once

#include "renderer.h"
#include <vector>
#include <glm/vec2.hpp>

class Freecell
{
public:
    void init();
    void mainLoop();
    void terminate();
    void processInput(double xPos, double yPos, bool isDraging, bool isDragStart);
    void processDoubleClick(double xPos, double yPos);

    glm::vec2 m_map[8][12];
    glm::vec2 m_openCellsMap[4];
    glm::vec2 m_foundationMap[4];

    std::vector<Card*> m_table[8];
    std::vector<Card*> m_openCells[4];
    std::vector<Card*> m_foundations[4];


    int selectedX = -1;
    int selectedY = -1;

private:
    void setBoardLayout();
    void createDeck();
    void createOpenCellsAndFoundations();
    void swap(int i, int j);
    void shuffle();
    void fillTable();
    bool isLegalMoveTable(std::vector<Card*>* stack, int src, int dst);
    bool isLegalMoveTable(std::vector<Card*>* stack, int srcX, int srcY, int dst);
    bool isLegalMoveFoundation(Card* card, int dst);
    void handleOpenCellsClick(int i, bool isDragStart);
    void handleFoundationsClick(int i, bool isDragStart);
    void handleTableClick(int i, int j, bool isDragStart);
    void deselect();
    bool moveCardToFoundations(std::vector<Card*>& src);
    bool moveCardToOpenCells(std::vector<Card*>& src);



    Card* m_selected{};
    std::vector<Card> m_deck;
    int m_numberOfOpenCells;
};
