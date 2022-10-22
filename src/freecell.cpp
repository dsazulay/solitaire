#include "freecell.h"
#include "utils/log.h"

void Freecell::init()
{
    setBoardLayout();
    createDeck();
    createOpenCellsAndFoundations();
    shuffle();
    fillTable();
}

void Freecell::setBoardLayout()
{
    float initPosX = 80;
    float offsetX = 160;
    float initPosY = 400;
    float offsetY = 32;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 12; j++)
        {
            m_map[i][j] = glm::vec2(initPosX + i * offsetX, initPosY - j * offsetY);
        }
    }

    for (int i = 0; i < 4; i++)
    {
        m_openCellsMap[i] = glm::vec2(initPosX + i * offsetX, 600);
    }

    initPosX += 4 * offsetX;
    for (int i = 0; i < 4; i++)
    {
        m_foundationMap[i] = glm::vec2(initPosX + i * offsetX, 600);
    }
}

void Freecell::fillTable()
{
    int index = 0;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            m_table[j].push_back(m_deck.at(index++));
            if (index > 51)
                return;
        }
    }
}

void Freecell::createDeck()
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 13; j++)
        {
            Card c;
            c.number = j;
            c.suit = i;
            c.color = i > 1;
            m_deck.push_back(c);
        }
    }
}

void Freecell::createOpenCellsAndFoundations()
{
    for (int i = 0; i < 4; i++)
    {
        m_openCells[i].push_back(m_deck.at(13));
    }

    m_foundations[0].push_back(m_deck.at(0));
    m_foundations[1].push_back(m_deck.at(13));
    m_foundations[2].push_back(m_deck.at(26));
    m_foundations[3].push_back(m_deck.at(39));
}

void Freecell::swap(int i, int j)
{
    auto tmp = m_deck[i];
    m_deck[i] = m_deck[j];
    m_deck[j] = tmp;
}

void Freecell::shuffle()
{
    for (int i = m_deck.size() - 1; i >= 0; i--)
    {
        int j = rand() % (i + 1);
        swap(i, j);
    }
}

bool Freecell::isLegalMoveTable(int src, int dst)
{
    bool diffColor = m_table[src].back().color != m_table[dst].back().color;
    bool nextNumber = m_table[src].back().number == m_table[dst].back().number - 1;

    return diffColor && nextNumber;
}

void Freecell::processInput(double xpos, double ypos)
{
    int cardSize = 100;
    int cardHeight = 180;

    //LOG_INFO(xpos << " " << ypos);

    for (int i = 0; i < 8; i++)
    {
        if (xpos > m_map[i][0].x - 50 && xpos < m_map[i][0].x - 50 + cardSize)
        {
            if (ypos > 121 - 74 && ypos < 121 + 74)
            {
                if (xpos < 700)
                {
                    if (selectedX != -1)
                    {
                        if (selectedY == 20)
                        {
                            m_openCells[i].push_back(m_openCells[selectedX].back());
                            m_openCells[selectedX].pop_back();
                        }
                        else
                        {
                            m_openCells[i].push_back(m_table[selectedX].back());
                            m_table[selectedX].pop_back();
                        }
                        selectedX = -1;
                        selectedY = -1;
                        return;
                    }

                    selectedX = i;
                    selectedY = 20;
                    return;
                }
                else
                {
                    LOG_INFO("foundations");
                    return;
                }
            }
            else
            {
                int stackSize = m_table[i].size();
                ypos = 720 - ypos;
                for (int j = 0; j < stackSize - 1; j++)
                {
                    if (ypos < m_map[i][j].y + 74 && ypos > m_map[i][j].y + 30)
                    {
                        LOG_INFO("no support for multiple selection");
                        return;
                    }
                }
                if (stackSize > 0 && ypos < m_map[i][stackSize - 1].y + 74 && ypos > m_map[i][stackSize - 1].y - 74)
                {
                    if (selectedX != -1)
                    {
                        if (selectedY == 20)
                        {
                            m_table[i].push_back(m_openCells[selectedX].back());
                            m_openCells[selectedX].pop_back();
                        }
                        else
                        {
                            if (!isLegalMoveTable(selectedX, i))
                            {
                                LOG_INFO("Invalid move");
                            }
                            else
                            {
                                m_table[i].push_back(m_table[selectedX].back());
                                m_table[selectedX].pop_back();
                            }
                        }
                        selectedX = -1;
                        selectedY = -1;
                        return;
                    }

                    selectedX = i;
                    selectedY = stackSize - 1;
                    return;
                }
            }
        }
    }

}
