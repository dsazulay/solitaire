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
            m_board.table[j].back()->pos = glm::vec3(m_board.tableMap[j][i], 0.0);
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
    if (!isDragStart)
        return;
    
    if ((int) area[x].size() == 0)
    {
        LOG_INFO("Cannot select empty stack");
        return;
    }

    if (((int) area[x].size()) - 1 != y)
    {
        if (!checkSequence(x, y))
        {
            LOG_INFO("Cannot selected unsorted middle card");
            return;
        }
    }

    m_cardSelected.card = area[x][y];
    m_cardSelected.card->selectionTint = glm::vec3(0.7, 0.7, 0.9);
    m_cardSelected.area = area;
    m_cardSelected.x = x;
    m_cardSelected.y = y;
    m_cardSelected.pos = m_cardSelected.card->pos;

    std::span<Card*> cards(area[x].data() + y, area[x].size() - y);
    m_board.draggingAnimation.start(cards);
}

void Freecell::deselect()
{
    if (m_cardSelected.card == nullptr)
        return;

    m_cardSelected.card->selectionTint = glm::vec3(1, 1, 1);
    m_cardSelected.card = nullptr;
    m_cardSelected.area = nullptr;
    m_cardSelected.x = -1;
    m_cardSelected.y = -1;

    if (!m_board.draggingAnimation.isDone)
        m_board.draggingAnimation.stop();
}

void Freecell::handleOpenCellsClick(int i, bool isDragStart)
{
    if (m_cardSelected.card == nullptr)
    {
        select(m_board.openCells.data(), i, m_board.openCells[i].size() - 1, isDragStart);
        return;
    }

    if (!openCellsIsLegalMove(m_cardSelected.card, i))
    {
        LOG_INFO("(Open cells) Invalid move");

        std::span<Card*> cards(m_cardSelected.area[m_cardSelected.x].data() + m_cardSelected.y, m_cardSelected.area[m_cardSelected.x].size() - m_cardSelected.y);
        MovingAnimation m(cards, m_cardSelected.card->pos, m_cardSelected.pos);
        m_board.movingAnimation.push_back(m);

        deselect();
        return;
    }

    int col = m_cardSelected.x;

    std::span<Card*> cards(m_cardSelected.area[m_cardSelected.x].data() + m_cardSelected.y, 1);
    MovingAnimation m(cards, m_cardSelected.card->pos, m_board.openCellsMap[i]);
    m_board.movingAnimation.push_back(m);

    moveCard(m_cardSelected.area[col], m_board.openCells[i], 1);
    m_history.recordMove(&m_cardSelected.area[col], &m_board.openCells[i], 1, m_cardSelected.pos, m_board.openCellsMap[i]);

    deselect();
}

void Freecell::handleFoundationsClick(int i, bool isDragStart)
{
    if (m_cardSelected.card == nullptr)
    {
        select(m_board.foundations, i, m_board.foundations[i].size() - 1, isDragStart);
        return;
    }

    if (!foundationsIsLegalMove(m_cardSelected.card, i))
    {
        LOG_INFO("(Foundations) Invalid move");

        std::span<Card*> cards(m_cardSelected.area[m_cardSelected.x].data() + m_cardSelected.y, m_cardSelected.area[m_cardSelected.x].size() - m_cardSelected.y);
        MovingAnimation m(cards, m_cardSelected.card->pos, m_cardSelected.pos);
        m_board.movingAnimation.push_back(m);

        deselect();
        return;
    }

    int col = m_cardSelected.x;
    
    std::span<Card*> cards(m_cardSelected.area[m_cardSelected.x].data() + m_cardSelected.y, 1);
    MovingAnimation m(cards, m_cardSelected.card->pos, m_board.foundationMap[i]);
    m_board.movingAnimation.push_back(m);

    moveCard(m_cardSelected.area[col], m_board.foundations[i], 1);
    m_history.recordMove(&m_cardSelected.area[col], &m_board.foundations[i], 1, m_cardSelected.pos, m_board.foundationMap[i]);

    deselect();

    if (checkWin())
        LOG_INFO("You Won!");

}

void Freecell::handleTableClick(int i, int j, bool isDragStart)
{
    if (m_cardSelected.card == nullptr)
    {
        select(m_board.table, i, j, isDragStart);
        return;
    }

    if (!tableIsLegalMove(m_cardSelected.card, i))
    {
        LOG_INFO("(Table) Invalid move");

        std::span<Card*> cards(m_cardSelected.area[m_cardSelected.x].data() + m_cardSelected.y, m_cardSelected.area[m_cardSelected.x].size() - m_cardSelected.y);
        MovingAnimation m(cards, m_cardSelected.card->pos, m_cardSelected.pos);
        m_board.movingAnimation.push_back(m);
        
        deselect();
        return;
    }

    int col = m_cardSelected.x;
    int row = m_cardSelected.y;
    int diff = ((int)m_cardSelected.area[col].size()) - row;

    std::span<Card*> cards(m_cardSelected.area[col].data() + row, diff);
    MovingAnimation m(cards, m_cardSelected.card->pos, m_board.tableMap[i][m_board.table[i].size()]);
    m_board.movingAnimation.push_back(m);

    moveCard(m_cardSelected.area[col], m_board.table[i], diff);
    m_history.recordMove(&m_cardSelected.area[col], &m_board.table[i], diff, m_cardSelected.pos, m_board.tableMap[i][m_board.table[i].size() - diff]);

    deselect();
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
            std::span<Card*> cards(m_cardSelected.area[m_cardSelected.x].data() + m_cardSelected.y, m_cardSelected.area[m_cardSelected.x].size() - m_cardSelected.y);
            MovingAnimation m(cards, m_cardSelected.card->pos, m_cardSelected.pos);
            m_board.movingAnimation.push_back(m);

            deselect();
        }
        return;
    }

    // top area
    if (yPos > 600 - 74 && yPos < 600 + 74)
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
                std::span<Card*> cards(m_cardSelected.area[m_cardSelected.x].data() + m_cardSelected.y, m_cardSelected.area[m_cardSelected.x].size() - m_cardSelected.y);
                MovingAnimation m(cards, m_cardSelected.card->pos, m_cardSelected.pos);
                m_board.movingAnimation.push_back(m);
                
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
    if (yPos > 600 - 74 && yPos < 600 + 74)
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

            tryMoveFromTo(m_board.openCells[i], m_board.foundations, i, m_board.foundationMap, &Freecell::foundationsIsLegalMove);

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

        if (!tryMoveFromTo(m_board.table[i], m_board.foundations, i, m_board.foundationMap, &Freecell::foundationsIsLegalMove))
        {
            tryMoveFromTo(m_board.table[i], m_board.openCells, i, m_board.openCellsMap, &Freecell::openCellsIsLegalMove);
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
    if (m_history.isUndoStackEmpty())
    {
        LOG_WARN("No moves to undo");
        return;
    }
    Move move = m_history.getTopUndoMove();
    
    int index = move.dstStack->size() - move.cardQuantity;
    std::span<Card*> cards(move.dstStack->data() + index, move.cardQuantity);
    MovingAnimation m(cards, move.dstPos, move.srcPos);
    m_board.movingAnimation.push_back(m);

    moveCard(*move.dstStack, *move.srcStack, move.cardQuantity);
    m_history.undo();
}

void Freecell::redoMove()
{
    if (m_history.isRedoStackEmpty())
    {
        LOG_WARN("No moves to redo");
        return;
    }
    Move move = m_history.getTopRedoMove();

    int index = move.dstStack->size() - move.cardQuantity;
    std::span<Card*> cards(move.dstStack->data() + index, move.cardQuantity);
    MovingAnimation m(cards, move.dstPos, move.srcPos);
    m_board.movingAnimation.push_back(m);

    moveCard(*move.dstStack, *move.srcStack, move.cardQuantity);
    m_history.redo();
}

bool Freecell::tryMoveFromTo(std::vector<Card*>& src, std::span<std::vector<Card*>> dst, int col, std::span<glm::vec2> dstAreaPos, bool(Freecell::*isLegalMove)(Card* card, int c))
{
    for (int i = 0; i < 4; i++)
    {
        if ((this->*isLegalMove)(src.back(), i))
        {
            std::span<Card*> cards(src.data() + src.size() - 1, 1);
            MovingAnimation m(cards, cards.back()->pos, dstAreaPos[i]);
            m_board.movingAnimation.push_back(m);

            moveCard(src, dst[i], 1);
            m_history.recordMove(&src, &dst[i], 1, cards.back()->pos, dstAreaPos[i]);

            // TODO: check if it's necessary to use on complete for animations
            // glm::vec3 srcCardPos = cards.back()->pos;
            // glm::vec2 dstPos = dstAreaPos[i];
            // MovingAnimation m(cards, cards.back()->pos, dstAreaPos[i], [&, dst, i, srcCardPos, dstPos]()
            // {
            //     moveCard(src, dst[i], 1);
            //     m_history.recordMove(&src, &dst[i], 1, srcCardPos, dstPos);
            // });
            // m_board.movingAnimation.push_back(m);
            
            return true;
        }
    }

    LOG_INFO("No valid move for open cells or foundations");
    deselect();
    return false;
}

void Freecell::update()
{
    for (std::vector<MovingAnimation>::iterator it = m_board.movingAnimation.begin(); it != m_board.movingAnimation.end();)
    {
        it->update();
        if (it->isDone)
            m_board.movingAnimation.erase(it);
        else
            ++it;
    }
    if (!m_board.draggingAnimation.isDone)
        m_board.draggingAnimation.update(); 
}

bool Freecell::openCellsIsLegalMove(Card* card, int col)
{
    if (m_cardSelected.card != nullptr)
    {
        int cardSelectedCol = m_cardSelected.x;
        if (m_cardSelected.y != ((int) m_cardSelected.area[cardSelectedCol].size()) - 1)
            return false;
    }

    return m_board.openCells[col].size() == 0;
}

bool Freecell::foundationsIsLegalMove(Card* card, int col)
{
    if (m_cardSelected.card != nullptr)
    {
        int cardSelectedCol = m_cardSelected.x;
        if (m_cardSelected.y != ((int) m_cardSelected.area[cardSelectedCol].size()) - 1)
            return false;
    }

    if (m_board.foundations[col].size() == 0)
        return card->number == 0;

    return m_board.foundations[col].back()->number == card->number - 1
        && m_board.foundations[col].back()->suit == card->suit;
}

bool Freecell::tableIsLegalMove(Card* card, int col)
{
    int cardSelectedCol = m_cardSelected.x;
    int cardSelectedRow = m_cardSelected.y;
    int diff = ((int)m_cardSelected.area[cardSelectedCol].size()) - cardSelectedRow;
    if (getMaxCardsToMove(m_board.table[col].empty()) < diff)
        return false;

    if (m_board.table[col].size() == 0)
        return true;

    bool diffColor =  m_board.table[col].back()->suit % 2 != card->suit % 2;
    bool nextNumber = m_board.table[col].back()->number - 1 == card->number;

    return diffColor && nextNumber;   
}

void Freecell::moveCard(std::vector<Card*>& src, std::vector<Card*>& dst, int n)
{
    for (int i = src.size() - n; i < (int) src.size(); i++)
    {
        dst.push_back(src[i]);
    }

    for (int i = 0; i < n; i++)
    {
        src.pop_back();
    }
}

void History::recordMove(std::vector<Card*>* src, std::vector<Card*>* dst, int n, glm::vec2 srcPos, glm::vec2 dstPos)
{
    Move move;
    move.srcStack = src;
    move.dstStack = dst;
    move.cardQuantity = n;
    move.srcPos = srcPos;
    move.dstPos = dstPos;

    m_undoStack.push(move);
    if (m_redoStack.size() > 0)
        m_redoStack = std::stack<Move>();
}

void History::undo()
{
    if (m_undoStack.empty())
    {
        LOG_WARN("No moves to undo");
        return;
    }

    Move move = m_undoStack.top();

    Move redoMove;
    redoMove.srcStack = move.dstStack;
    redoMove.dstStack = move.srcStack;
    redoMove.cardQuantity = move.cardQuantity;
    redoMove.srcPos = move.dstPos;
    redoMove.dstPos = move.srcPos;
    
    m_undoStack.pop();
    m_redoStack.push(redoMove);
}

void History::redo()
{
    if (m_redoStack.empty())
    {
        LOG_WARN("No moves to redo");
        return;
    }

    Move move = m_redoStack.top();

    Move undoMove;
    undoMove.srcStack = move.dstStack;
    undoMove.dstStack = move.srcStack;
    undoMove.cardQuantity = move.cardQuantity;
    undoMove.srcPos = move.dstPos;
    undoMove.dstPos = move.srcPos;

    m_redoStack.pop();
    m_undoStack.push(undoMove);
}

bool History::isUndoStackEmpty() const
{
    return m_undoStack.empty();
}

bool History::isRedoStackEmpty() const
{
    return m_redoStack.empty();
}

Move History::getTopUndoMove() const
{
    return m_undoStack.top();
}

Move History::getTopRedoMove() const
{
    return m_redoStack.top();
}
