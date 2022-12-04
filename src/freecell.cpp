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
            m_board.tableMap[i][j] = glm::vec2(initPosX + i * offsetX, initPosY - j * offsetY);
        }
    }

    for (int i = 0; i < 4; i++)
    {
        m_board.openCellsMap[i] = glm::vec2(initPosX + i * offsetX, 600);
    }

    initPosX += 4 * offsetX;
    for (int i = 0; i < 4; i++)
    {
        m_board.foundationMap[i] = glm::vec2(initPosX + i * offsetX, 600);
    }
}

void Freecell::fillTable()
{
    int index = 0;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            m_board.table[j].push_back(&m_deck.at(index++));
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
    m_board.openCellsBg = &m_deck.at(52);
    m_board.foundationsBg = &m_deck.at(53);
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

Board& Freecell::board()
{
    return m_board;
}

void Freecell::select(std::vector<Card*>* area, int x, int y, bool isDragStart)
{
      m_cardSelected.card = area[x][y];
      m_cardSelected.card->selectionTint = glm::vec3(0.7, 0.7, 0.9);
      m_cardSelected.card->dragging = isDragStart;
      m_cardSelected.card->shouldSetOffset = isDragStart;
      m_cardSelected.area = area;
      m_cardSelected.x = x;
      m_cardSelected.y = y;

      for (int i = y + 1; i < (int) area[x].size(); i++)
      {
        area[x][i]->dragging = isDragStart;
        area[x][i]->shouldSetOffset = isDragStart;
      }
}

void Freecell::deselect()
{
    if (m_cardSelected.card == nullptr)
        return;

    m_cardSelected.card->selectionTint = glm::vec3(1, 1, 1);
    m_cardSelected.card->dragging = false;
    m_cardSelected.card->shouldSetOffset = false;
    m_cardSelected.card = nullptr;
    m_cardSelected.area = nullptr;
    m_cardSelected.x = -1;
    m_cardSelected.y = -1;
}

void Freecell::deselectTrailingCards()
{
    if (m_cardSelected.card == nullptr)
        return;
    
    std::vector<Card*> cardStack = m_cardSelected.area[m_cardSelected.x];
    for (int i = m_cardSelected.y + 1; i < (int) cardStack.size(); i++)
      {
        cardStack[i]->dragging = false;
        cardStack[i]->shouldSetOffset = false;
      }
}

bool Freecell::isLegalMoveTable(std::vector<Card*>* stack, int srcX, int srcY, int dst)
{
    if (m_board.table[dst].size() == 0)
        return true;

    bool diffColor = stack[srcX][srcY]->suit % 2 != m_board.table[dst].back()->suit % 2;
    bool nextNumber = stack[srcX][srcY]->number == m_board.table[dst].back()->number - 1;

    return diffColor && nextNumber;
}

bool Freecell::isLegalMoveFoundation(Card* card, int dst)
{
    if (m_board.foundations[dst].size() == 0)
        return card->number == 0;

    return m_board.foundations[dst].back()->number == card->number - 1
        && m_board.foundations[dst].back()->suit == card->suit;
}

void Freecell::handleOpenCellsClick(int i, bool isDragStart)
{
    if (m_cardSelected.card == nullptr)
    {
        if (!isDragStart)
            return;

        if (m_board.openCells[i].size() > 0)
            select(m_board.openCells, i, m_board.openCells[i].size() - 1, isDragStart);
        else
            LOG_INFO("Cannot select empty open cells stack");

        return;
    }

    if (m_board.openCells[i].size() > 0)
    {
        LOG_INFO("Open cell occupied");
        deselectTrailingCards();
        deselect();
        return;
    }

    int col = m_cardSelected.x;
    if (m_cardSelected.y != ((int) m_cardSelected.area[col].size()) - 1)
    {
        LOG_INFO("Cannot move two cards at the same time");
        deselectTrailingCards();
        deselect();
        return;
    }

    m_board.openCells[i].push_back(m_cardSelected.area[col].back());
    m_cardSelected.area[col].pop_back();
    
    Move move;
    move.srcStack = &m_cardSelected.area[col];
    move.dstStack = &m_board.openCells[i];
    move.cardQuantity = 1;
    m_undoStack.push(move);

    deselect();
}

void Freecell::handleFoundationsClick(int i, bool isDragStart)
{
    if (m_cardSelected.card == nullptr)
    {
        if (!isDragStart)
            return;

        if (m_board.foundations[i].size() > 0)
            select(m_board.foundations, i, m_board.foundations[i].size() - 1, isDragStart);
        else
            LOG_INFO("Cannot select empty foundations stack");

        return;
    }

    int col = m_cardSelected.x;
    if (m_cardSelected.y != ((int) m_cardSelected.area[col].size()) - 1)
    {
        LOG_INFO("Cannot move two cards at the same time");
        deselectTrailingCards();
        deselect();
        return;
    }

    if (!isLegalMoveFoundation(m_cardSelected.area[col].back(), i))
    {
        LOG_INFO("Invalid foundation move");
        deselect();
        return;
    }

    m_board.foundations[i].push_back(m_cardSelected.area[col].back());
    m_cardSelected.area[col].pop_back();

    Move move;
    move.srcStack = &m_cardSelected.area[col];
    move.dstStack = &m_board.foundations[i];
    move.cardQuantity = 1;

    m_undoStack.push(move);
    m_redoStack = std::stack<Move>();

    deselect();

    if (checkWin())
        LOG_INFO("You Won!");

}

bool Freecell::checkSequence(int i, int j)
{
    std::vector<Card*> stack = m_board.table[i];
    int currentCard = ((int)stack.size()) - 1;

    for (int n = currentCard; n > j; n--)
    {
        bool diffColor = stack[n]->suit % 2 != stack[n - 1]->suit % 2;
        bool nextNumber = stack[n]->number == stack[n - 1]->number - 1;

        if (!(diffColor && nextNumber))
            return false;
    }

    return true;
}

bool Freecell::checkWin()
{
    for (int i = 0; i < 8; i++)
    {
        if (!checkSequence(i, 0))
            return false;
    }

    return true;
}

int Freecell::getMaxCardsToMove(bool movingToEmptySpace)
{
    // Initialize with one because you can always move one card
    int emptyOpenCells = 1;
    for (int i = 0; i < 4; i++)
    {
        emptyOpenCells += (int) m_board.openCells[i].empty();
    }

    int emptyTableColumns = movingToEmptySpace ? -1 : 0;
    for (int i = 0; i < 8; i++)
    {
        emptyTableColumns += (int) m_board.table[i].empty();
    }

    return emptyTableColumns * emptyOpenCells + emptyOpenCells;
}

void Freecell::handleTableClick(int i, int j, bool isDragStart)
{
    if (m_cardSelected.card == nullptr)
    {
        if (!isDragStart)
            return;

        if (m_board.table[i].size() == 0)
        {
            LOG_INFO("Cannot select empty table stack");
            return;
        }

        if (((int)m_board.table[i].size()) - 1 != j)
        {
            if (!checkSequence(i, j))
            {
                LOG_INFO("Cannot selected unsorted middle card");
                return;
            }
        }
        select(m_board.table, i, j, isDragStart);
        return;
    }

    int col = m_cardSelected.x;
    int row = m_cardSelected.y;
    if (!isLegalMoveTable(m_cardSelected.area, col, m_cardSelected.y, i))
    {
        LOG_INFO("Invalid table move");
        deselectTrailingCards();
        deselect();
        return;
    }

    // multiple cards move
    if (row != ((int) m_cardSelected.area[col].size()) - 1)
    {
        int diff = ((int)m_cardSelected.area[col].size()) - row;
        if (getMaxCardsToMove(m_board.table[i].empty()) >= diff)
        {
            // need to deselect first because the trailing selection 
            // is based on current position
            deselectTrailingCards();
            
            for (int n = row; n < (int) m_cardSelected.area[col].size(); n++)
            {
                m_board.table[i].push_back(m_cardSelected.area[col][n]);
            }

            for (int n = 0; n < diff; n++)
            {
                m_cardSelected.area[col].pop_back();
            }

            Move move;
            move.srcStack = &m_cardSelected.area[col];
            move.dstStack = &m_board.table[i];
            move.cardQuantity = diff;

            m_undoStack.push(move);
            m_redoStack = std::stack<Move>();
        }
        else
        {
            deselectTrailingCards();
            LOG_INFO("Need more open cells"); 
        }       

        deselect();
        return;
    }


    m_board.table[i].push_back(m_cardSelected.area[col].back());
    m_cardSelected.area[col].pop_back();
    
    Move move;
    move.srcStack = &m_cardSelected.area[col];
    move.dstStack = &m_board.table[i];
    move.cardQuantity = 1;

    m_undoStack.push(move);
    m_redoStack = std::stack<Move>();

    deselect();

}

int Freecell::getIndexX(int n, double xPos)
{
    for (int i = 0; i < n; i++)
    {
        if (xPos > m_board.tableMap[i][0].x - 50 && xPos < m_board.tableMap[i][0].x + 50)
            return i;
    }
    return -1;
}

int Freecell::getIndexY(int n, int col, double yPos)
{
    if (n == 0)
    {
        if (yPos < m_board.tableMap[col][0].y + 74 && yPos > m_board.tableMap[col][0].y - 74)
            return 0;
        else
            return -1;
    }

    for (int i = 0; i < n - 1; i++)
    {
        if (yPos < m_board.tableMap[col][i].y + 74 && yPos > m_board.tableMap[col][i].y + 42)
            return i;
    }


    if (yPos < m_board.tableMap[col][n - 1].y + 74 && yPos > m_board.tableMap[col][n - 1].y - 74)
        return n - 1;

    return -1;
}

void Freecell::processInput(double xPos, double yPos, bool isDraging, bool isDragStart)
{
    int i = getIndexX(8, xPos);
    if (i == -1)
    {
        if (m_cardSelected.card != nullptr)
        {
            deselectTrailingCards();
            deselect();
        }
        return;
    }

    // top area
    if (yPos > 579 - 74 && yPos < 579 + 74)
    {
        // open cells
        if (xPos < 640)
            handleOpenCellsClick(i, isDragStart);
        else
            handleFoundationsClick(i - 4, isDragStart);
    }
    else
    {
        int stackSize = m_board.table[i].size();
        int j = getIndexY(stackSize, i, yPos);
        if (j == -1)
        {
            if (m_cardSelected.card != nullptr)
            {
                deselectTrailingCards();
                deselect();
            }
            return;
        }

        handleTableClick(i, j, isDragStart);
    }
}

void Freecell::processDoubleClick(double xPos, double yPos)
{
    // top area
    if (yPos > 579 - 74 && yPos < 579 + 74)
    {
        // open cells
        if (xPos < 640)
        {
            int i = getIndexX(4, xPos);
            if (i == -1)
                return;

            if (m_board.openCells[i].size() == 0)
            {
                LOG_WARN("No action for double click an empty open cells stack");
                return;
            }

            moveCardToFoundations(m_board.openCells[i]);

            if (checkWin())
                LOG_INFO("You Won!");
        }
        // foundations
        else
        {
            LOG_WARN("No action for double click on foundations");
        }
    }
    else
    {
        int i = getIndexX(8, xPos);
        if (i == -1)
            return;

        int stackSize = m_board.table[i].size();
        int j = getIndexY(stackSize, i, yPos);
        if (j == -1)
            return;

        if (j < stackSize - 1)
        {
            LOG_WARN("No action for double click on a middle card of a table stack");
            return;
        }

        if (stackSize == 0)
        {
            LOG_WARN("No action for double click on an empty table stack");
            return;
        }


        if(!moveCardToFoundations(m_board.table[i]))
        {
            moveCardToOpenCells(m_board.table[i]);
        }
        else
        {
            if (checkWin())
                LOG_INFO("You Won!");
        }
    }
}

void Freecell::undoMove()
{
    if (m_undoStack.empty())
    {
        LOG_WARN("No moves to undo");
        return;
    }

    Move move = m_undoStack.top();
    int index = move.dstStack->size() - move.cardQuantity;

    for (int i = index; i < (int) move.dstStack->size(); i++)
    {
        move.srcStack->push_back((*move.dstStack)[i]);
    }

    for (int i = 0; i < move.cardQuantity; i++)
    {
        move.dstStack->pop_back();
    }

    Move redoMove;
    redoMove.srcStack = move.dstStack;
    redoMove.dstStack = move.srcStack;
    redoMove.cardQuantity = move.cardQuantity;

    m_undoStack.pop();
    m_redoStack.push(redoMove);
}

void Freecell::redoMove()
{
    if (m_redoStack.empty())
    {
        LOG_WARN("No moves to redo");
        return;
    }

    Move move = m_redoStack.top();
    int index = move.dstStack->size() - move.cardQuantity;

    for (int i = index; i < (int) move.dstStack->size(); i++)
    {
        move.srcStack->push_back((*move.dstStack)[i]);
    }

    for (int i = 0; i < move.cardQuantity; i++)
    {
        move.dstStack->pop_back();
    }

    Move undoMove;
    undoMove.srcStack = move.dstStack;
    undoMove.dstStack = move.srcStack;
    undoMove.cardQuantity = move.cardQuantity;

    m_redoStack.pop();
    m_undoStack.push(undoMove);
}

bool Freecell::moveCardToFoundations(std::vector<Card*>& src)
{
    for (int i  = 0; i < 4; i++)
    {
        if (isLegalMoveFoundation(src.back(), i))
        {
            m_board.foundations[i].push_back(src.back());
            src.pop_back();

            Move move;
            move.srcStack = &src;
            move.dstStack = &m_board.foundations[i];
            move.cardQuantity = 1;

            m_undoStack.push(move);
            m_redoStack = std::stack<Move>();

            return true;
        }
    }

    LOG_INFO("No valid move for foundations");
    deselect();
    return false;

}

bool Freecell::moveCardToOpenCells(std::vector<Card*>& src)
{
    for (int i  = 0; i < 4; i++)
    {
        if (m_board.openCells[i].size() == 0)
        {
            m_board.openCells[i].push_back(src.back());
            src.pop_back();

            Move move;
            move.srcStack = &src;
            move.dstStack = &m_board.openCells[i];
            move.cardQuantity = 1;

            m_undoStack.push(move);
            m_redoStack = std::stack<Move>();

            return true;
        }
    }

    LOG_INFO("No valid move for open cells");
    deselect();
    return false;
}
