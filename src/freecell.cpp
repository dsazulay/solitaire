#include "freecell.h"

#include "utils/log.h"
#include <_types/_uint8_t.h>

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
    m_selected->dragging = false;
    selectedX = -1;
    selectedY = -1;
}

bool Freecell::isLegalMoveTable(std::vector<Card*>* stack, int src, int dst)
{
    if (m_table[dst].size() == 0)
        return true;


    bool diffColor = stack[src].back()->suit % 2 != m_table[dst].back()->suit % 2;
    bool nextNumber = stack[src].back()->number == m_table[dst].back()->number - 1;

    return diffColor && nextNumber;
}

bool Freecell::isLegalMoveTable(std::vector<Card*>* stack, int srcX, int srcY, int dst)
{
    if (m_table[dst].size() == 0)
        return true;

    bool diffColor = stack[srcX][srcY]->suit % 2 != m_table[dst].back()->suit % 2;
    bool nextNumber = stack[srcX][srcY]->number == m_table[dst].back()->number - 1;

    return diffColor && nextNumber;
}

bool Freecell::isLegalMoveFoundation(Card* card, int dst)
{
    if (m_foundations[dst].size() == 1)
        return card->number == 0;

    return m_foundations[dst].back()->number == card->number - 1
        && m_foundations[dst].back()->suit == card->suit;
}

void Freecell::handleOpenCellsClick(int i, bool isDragStart)
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
    m_selected->dragging = isDragStart;
    m_selected->shouldSetOffset = isDragStart;
    selectedX = i;
    selectedY = 20;
    return;
}

void Freecell::handleFoundationsClick(int i, bool isDragStart)
{
    if (selectedX != -1)
    {
        if (selectedY == 20)
        {
            if (!isLegalMoveFoundation(m_openCells[selectedX].back(), i))
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
            if (!isLegalMoveFoundation(m_foundations[selectedX].back(), i))
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

            if (!isLegalMoveFoundation(m_table[selectedX].back(), i))
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
    m_selected->dragging = isDragStart;
    m_selected->shouldSetOffset = isDragStart;
    selectedX = i;
    selectedY = 30;
    return;
}

void Freecell::handleTableClick(int i, int j, bool isDragStart)
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
                LOG_INFO("Need more open cells");
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
    m_selected->dragging = isDragStart;
    m_selected->shouldSetOffset = isDragStart;
    selectedX = i;
    selectedY = j;
    return;
}

void Freecell::processInput(double xPos, double yPos, bool isDraging, bool isDragStart)
{
    int cardSize = 100;
    //int cardHeight = 180;

    for (int i = 0; i < 8; i++)
    {
        if (xPos > m_map[i][0].x - 50 && xPos < m_map[i][0].x - 50 + cardSize)
        {
            if (yPos > 121 - 74 && yPos < 121 + 74)
            {
                if (xPos < 700)
                {
                    handleOpenCellsClick(i, isDragStart);
                    return;
                }
                else
                {
                    handleFoundationsClick(i - 4, isDragStart);
                    return;
                }
            }
            else
            {
                int stackSize = m_table[i].size();
                yPos = 720 - yPos;
                for (int j = 0; j < stackSize - 1; j++)
                {
                    if (yPos < m_map[i][j].y + 74 && yPos > m_map[i][j].y + 30)
                    {
                        handleTableClick(i, j, isDragStart);
                        return;
                    }
                }
                if (stackSize > 0 && yPos < m_map[i][stackSize - 1].y + 74 && yPos > m_map[i][stackSize - 1].y - 74)
                {
                    handleTableClick(i, stackSize - 1, isDragStart);
                    return;
                }
                else if (stackSize == 0)
                {
                    handleTableClick(i, stackSize - 1, isDragStart);
                    return;
                }
            }
        }
    }
    if (m_selected != nullptr)
    {
        deselect();
    }
}

void Freecell::processDoubleClick(double xPos, double yPos)
{
    int cardSize = 100;
    //int cardHeight = 180;

    for (int i = 0; i < 8; i++)
    {
        if (xPos > m_map[i][0].x - 50 && xPos < m_map[i][0].x - 50 + cardSize)
        {
            if (yPos > 121 - 74 && yPos < 121 + 74)
            {
                if (xPos < 700)
                {
                    if (m_openCells[i].size() == 1)
                    {
                        LOG_INFO("Cannot double click empty stack");
                        return;
                    }
                    if (moveCardToFoundations(m_openCells[i]))
                        m_numberOfOpenCells++;
                }
                else
                {
                    LOG_INFO("No function for double click on foundations");
                }
            }
            else
            {
                int stackSize = m_table[i].size();
                yPos = 720 - yPos;
                for (int j = 0; j < stackSize - 1; j++)
                {
                    if (yPos < m_map[i][j].y + 74 && yPos > m_map[i][j].y + 30)
                    {
                        LOG_INFO("No function for double click on middle table stack");
                        return;
                    }
                }
                if (stackSize > 0 && yPos < m_map[i][stackSize - 1].y + 74 && yPos > m_map[i][stackSize - 1].y - 74)
                {
                    if(!moveCardToFoundations(m_table[i]))
                    {
                        if(moveCardToOpenCells(m_table[i]))
                            m_numberOfOpenCells--;
                    }
                }
                else if (stackSize == 0)
                {
                    LOG_INFO("No function for double click on empty table stack");
                }
            }
        }
    }
}

bool Freecell::moveCardToFoundations(std::vector<Card*>& src)
{
    for (uint8_t i  = 0; i < 4; i++)
    {
         if (isLegalMoveFoundation(src.back(), i))
         {
             m_foundations[i].push_back(src.back());
             src.pop_back();
             return true;
         }
    }

    LOG_INFO("No valid move for foundations");
    deselect();
    return false;

}

bool Freecell::moveCardToOpenCells(std::vector<Card*>& src)
{
    for (uint8_t i  = 0; i < 4; i++)
    {
        if (m_openCells[i].size() == 1)
        {
            m_openCells[i].push_back(src.back());
            src.pop_back();
            return true;
        }
    }

    LOG_INFO("No valid move for openCells");
    deselect();
    return false;
}
