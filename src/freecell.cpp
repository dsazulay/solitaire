#include "freecell.h"
#include "utils/log.h"
#include <_types/_uint8_t.h>
#include <vector>

void Freecell::init()
{
    setBoardLayout();
    createDeck();
    createOpenCellsAndFoundations();
    shuffle();
    fillTable();

    m_numberOfOpenCells = 4;
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
            m_table[j].push_back(&m_deck.at(index++));
            if (index > 51)
                return;
        }
    }
}

void Freecell::createDeck()
{
    unsigned int count = 0;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 13; j++)
        {
            Card c;
            c.number = j;
            c.suit = i;
            c.color = i % 2;
            c.selectionTint = glm::vec3(1,1,1);
            c.offsetX = count % 8;
            c.offsetY = (int)count * 0.125;
            m_deck.push_back(c);
            count++;
        }
    }

    // Add open cells and foundation background
    Card c;
    c.offsetX = 1;
    c.offsetY = 7;
    c.selectionTint = glm::vec3(1,1,1);
    m_deck.push_back(c);

    c.offsetX = 2;
    m_deck.push_back(c);
}

void Freecell::createOpenCellsAndFoundations()
{
    for (int i = 0; i < 4; i++)
    {
        m_openCells[i].push_back(&m_deck.at(52));
        m_foundations[i].push_back(&m_deck.at(53));
    }
}

void Freecell::swap(int i, int j)
{
    auto tmp = m_deck[i];
    m_deck[i] = m_deck[j];
    m_deck[j] = tmp;
}

void Freecell::shuffle()
{
    for (int i = m_deck.size() - 3; i >= 0; i--)
    {
        int j = rand() % (i + 1);
        swap(i, j);
    }
}

void Freecell::deselect()
{
    m_selected->selectionTint = glm::vec3(1,1,1);
    selectedX = -1;
    selectedY = -1;
}

bool Freecell::isLegalMoveTable(std::vector<Card*>* stack, int src, int dst)
{
    if (m_table[dst].size() == 0)
        return true;


    bool diffColor = stack[src].back()->color != m_table[dst].back()->color;
    bool nextNumber = stack[src].back()->number == m_table[dst].back()->number - 1;

    return diffColor && nextNumber;
}

bool Freecell::isLegalMoveTable(std::vector<Card*>* stack, int srcX, int srcY, int dst)
{
    if (m_table[dst].size() == 0)
        return true;

    bool diffColor = stack[srcX][srcY]->color != m_table[dst].back()->color;
    bool nextNumber = stack[srcX][srcY]->number == m_table[dst].back()->number - 1;

    return diffColor && nextNumber;
}

bool Freecell::isLegalMoveFoundation(std::vector<Card*>* stack, int src, int dst)
{
    if (m_foundations[dst].size() == 1)
        return stack[src].back()->number == 0;

    return m_foundations[dst].back()->number == stack[src].back()->number - 1
        && m_foundations[dst].back()->suit == stack[src].back()->suit;
}

void Freecell::handleOpenCellsClick(int i)
{
    if (selectedX != -1)
    {
        if (m_openCells[i].size() > 1)
        {
            LOG_INFO("Cannot move two cards on open cells");
            deselect();
            return;
        }

        if (selectedY == 20)
        {
            m_openCells[i].push_back(m_openCells[selectedX].back());
            m_openCells[selectedX].pop_back();
        }
        else if (selectedY == 30)
        {
            m_openCells[i].push_back(m_foundations[selectedX].back());
            m_foundations[selectedX].pop_back();
            m_numberOfOpenCells--;
        }
        else
        {

            if (selectedY != m_table[selectedX].size() - 1)
            {
                LOG_INFO("Cannot move more than two cards at the same time");
                deselect();
                return;
            }

            m_openCells[i].push_back(m_table[selectedX].back());
            m_table[selectedX].pop_back();
            m_numberOfOpenCells--;
        }
        deselect();
        return;
    }

    if (m_openCells[i].size() == 1)
    {
        LOG_INFO("Cannot select empty stack");
        return;
    }

    m_selected = m_openCells[i].back();
    m_selected->selectionTint = glm::vec3(0.7, 0.7, 0.9);
    selectedX = i;
    selectedY = 20;
    return;
}

void Freecell::handleFoundationsClick(int i)
{
    if (selectedX != -1)
    {
        if (selectedY == 20)
        {
            if (!isLegalMoveFoundation(m_openCells, selectedX, i))
            {
                LOG_INFO("Invalid foundation move");
                deselect();
                return;
            }
            m_foundations[i].push_back(m_openCells[selectedX].back());
            m_openCells[selectedX].pop_back();
            m_numberOfOpenCells++;
        }
        else if (selectedY == 30)
        {
            if (!isLegalMoveFoundation(m_foundations, selectedX, i))
            {
                LOG_INFO("Invalid foundation move");
                deselect();
                return;
            }
            m_foundations[i].push_back(m_foundations[selectedX].back());
            m_foundations[selectedX].pop_back();
        }
        else
        {
            if (selectedY != m_table[selectedX].size() - 1)
            {
                LOG_INFO("Cannot move more than two cards at the same time");
                deselect();
                return;
            }

            if (!isLegalMoveFoundation(m_table, selectedX, i))
            {
                LOG_INFO("Invalid foundation move");
                deselect();
                return;
            }
            m_foundations[i].push_back(m_table[selectedX].back());
            m_table[selectedX].pop_back();
        }
        deselect();
        return;
    }

    if (m_foundations[i].size() == 1)
    {
        LOG_INFO("Cannot select empty stack");
        return;
    }

    m_selected = m_foundations[i].back();
    m_selected->selectionTint = glm::vec3(0.7, 0.7, 0.9);
    selectedX = i;
    selectedY = 30;
    return;
}

void Freecell::handleTableClick(int i, int j)
{
    if (selectedX != -1)
    {
        if (selectedY == 20)
        {
            if (!isLegalMoveTable(m_openCells, selectedX, i))
            {
                LOG_INFO("Invalid move");
                deselect();
                return;
            }
            m_table[i].push_back(m_openCells[selectedX].back());
            m_openCells[selectedX].pop_back();
            m_numberOfOpenCells++;
        }
        else if (selectedY == 30)
        {
            if (!isLegalMoveTable(m_foundations, selectedX, i))
            {
                LOG_INFO("Invalid move");
                deselect();
                return;
            }
            m_table[i].push_back(m_foundations[selectedX].back());
            m_foundations[selectedX].pop_back();
        }
        else
        {
            if (!isLegalMoveTable(m_table, selectedX, selectedY, i))
            {
                LOG_INFO("Invalid move");
                deselect();
                return;
            }

            auto diff = m_table[selectedX].size() - selectedY;
            if (m_numberOfOpenCells >= diff - 1)
            {
                for (auto n = selectedY; n < m_table[selectedX].size(); n++)
                {
                    m_table[i].push_back(m_table[selectedX][n]);
                }

                for (auto n = 0; n < diff; n++)
                {
                    m_table[selectedX].pop_back();
                }
            }
            else
            {
                LOG_INFO("Need mor open cells");
                deselect();
                return;
            }

            //m_table[i].push_back(m_table[selectedX].back());
            //m_table[selectedX].pop_back();
        }
        deselect();
        return;
    }

    if (j == -1)
    {
        LOG_INFO("Cannot select empty stack");
        return;
    }

    m_selected = m_table[i][j];
    m_selected->selectionTint = glm::vec3(0.7, 0.7, 0.9);
    selectedX = i;
    selectedY = j;
    return;
}

void Freecell::processInput(double xpos, double ypos)
{
    int cardSize = 100;
    //int cardHeight = 180;

    for (int i = 0; i < 8; i++)
    {
        if (xpos > m_map[i][0].x - 50 && xpos < m_map[i][0].x - 50 + cardSize)
        {
            if (ypos > 121 - 74 && ypos < 121 + 74)
            {
                if (xpos < 700)
                {
                    handleOpenCellsClick(i);
                }
                else
                {
                    handleFoundationsClick(i - 4);
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
                        handleTableClick(i, j);
                        return;
                    }
                }
                if (stackSize > 0 && ypos < m_map[i][stackSize - 1].y + 74 && ypos > m_map[i][stackSize - 1].y - 74)
                {
                    handleTableClick(i, stackSize - 1);
                }
                else if (stackSize == 0)
                {
                    handleTableClick(i, stackSize - 1);
                }
            }
        }
    }
}
