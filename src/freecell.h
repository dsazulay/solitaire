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
    void processInput(double xpos, double ypos);

    glm::vec2 m_map[8][12];
    glm::vec2 m_openCellsMap[4];
    glm::vec2 m_foundationMap[4];

    std::vector<Card> m_table[8];
    std::vector<Card> m_openCells[4];
    std::vector<Card> m_foundations[4];


    int selectedX = -1;
    int selectedY = -1;

private:
    void setBoardLayout();
    void createDeck();
    void createOpenCellsAndFoundations();
    void swap(int i, int j);
    void shuffle();
    void fillTable();
    bool isLegalMoveTable(int src, int dst);

    std::vector<Card> m_deck;
};
