#include "freecell.h"

#include <filesystem>

#include "event.h"
#include "dispatcher.h"
#include "utils/log.h"
#include "serializer.h"

struct LayoutSize
{
    const float halfScreenWidth = 640.0f;
    const float halfCardWidth = 50.0f;
    const float halfCardHeight = 74.0f;
    const float topAreaPosY = 600.0f;
};

const LayoutSize LAYOUT_SIZE;

void Freecell::init()
{
    loadPlayerData();
    setBoardLayout();
    createOpenCellsAndFoundations();
    m_dealer.shuffleDeck();
    m_dealer.fillTableau(m_board.tableau, m_boardMap.tableau);

    m_currentState = GameState::Playing;
    m_matchData.startTime = Timer::time;
}

void Freecell::update()
{
    std::vector<MovingAnimation>::iterator it = m_movingAnimation.begin();
    if (it != m_movingAnimation.end())
    {
        it->update();
        if (it->isDone())
            m_movingAnimation.erase(it);
    }
    if (!m_draggingAnimation.isDone())
        m_draggingAnimation.update(); 
    
    if (m_currentState == GameState::Playing)
    {
        m_matchData.currentTime = Timer::time - m_matchData.startTime;
    }
    else if (m_currentState == GameState::WinAnimation)
    {
        if (!isComplete() && m_movingAnimation.size() == 0)
            playWinAnimation();
        else if(isComplete())
        {
            m_currentState = GameState::Won;
            updatePlayerData(true, m_matchData.currentTime);
            GameWinEvent e;
            Dispatcher<GameWinEvent>::post(e);
        }
    }
    else if (m_currentState == GameState::Won)
    {
        // do nothing
    }
}

void Freecell::handleInputClick(double xPos, double yPos, bool isDraging, bool isDragStart)
{
    if (m_movingAnimation.size() > 0)
    {
        LOG_WARN("Can't input while card is moving!");
        return;
    }

    // top area
    if (yPos > LAYOUT_SIZE.topAreaPosY - LAYOUT_SIZE.halfCardHeight && yPos < LAYOUT_SIZE.topAreaPosY + LAYOUT_SIZE.halfCardHeight)
    {
        // open cells
        if (xPos < LAYOUT_SIZE.halfScreenWidth)
        {
            int i = getTopAreaIndexX(m_boardMap.openCells, xPos);
            if (i == -1)
            {
                moveBackAndDeselectCard();
                return;
            }
            std::span<glm::vec2> dtsAreaPos(&m_boardMap.openCells[i], 1);
            handleClick(m_board.openCells[i], dtsAreaPos, i, m_board.openCells[i].size() - 1, &Freecell::openCellsIsLegalMove, isDragStart);
        }
        else
        {
            int i = getTopAreaIndexX(m_boardMap.foundations, xPos);
            if (i == -1)
            {
                moveBackAndDeselectCard();
                return;
            }
            std::span<glm::vec2> dtsAreaPos(&m_boardMap.foundations[i], 1);
            handleClick(m_board.foundations[i], dtsAreaPos, i, m_board.foundations[i].size() - 1, &Freecell::foundationsIsLegalMove, isDragStart);
        }
    }
    else
    {
        int i = getIndexX(8, xPos);
        if (i == -1)
        {
            moveBackAndDeselectCard();
            return;
        }
        int stackSize = m_board.tableau[i].size();
        int j = getIndexY(stackSize, i, yPos);
        if (j == -1)
        {
            moveBackAndDeselectCard();
            return;
        }

        std::span<glm::vec2> dtsAreaPos(m_boardMap.tableau[i]);
        handleClick(m_board.tableau[i], dtsAreaPos, i, j, &Freecell::tableIsLegalMove, isDragStart);
    }
}

void Freecell::handleInputDoubleClick(double xPos, double yPos)
{
    if (m_movingAnimation.size() > 0)
    {
        LOG_WARN("Can't input while card is moving!");
        return;
    }
    // top area
    if (yPos > LAYOUT_SIZE.topAreaPosY - LAYOUT_SIZE.halfCardHeight && yPos < LAYOUT_SIZE.topAreaPosY + LAYOUT_SIZE.halfCardHeight)
    {
        // open cells
        if (xPos < LAYOUT_SIZE.halfScreenWidth)
        {
            int i = getTopAreaIndexX(m_boardMap.openCells, xPos);
            if (i == -1)
                return;

            if (m_board.openCells[i].size() == 0)
            {
                LOG_WARN("No action for double click an empty open cells stack");
                return;
            }

            tryMoveFromTo(m_board.openCells[i], m_board.foundations, m_boardMap.foundations, &Freecell::foundationsIsLegalMove);
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

        int stackSize = m_board.tableau[i].size();
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

        if (!tryMoveFromTo(m_board.tableau[i], m_board.foundations, m_boardMap.foundations, &Freecell::foundationsIsLegalMove))
        {
            tryMoveFromTo(m_board.tableau[i], m_board.openCells, m_boardMap.openCells, &Freecell::openCellsIsLegalMove);
        }
    }
}

void Freecell::handleInputUndo()
{
    if (m_movingAnimation.size() > 0)
    {
        LOG_WARN("Can't input while card is moving!");
        return;
    }

    if (m_history.isUndoStackEmpty())
    {
        LOG_WARN("No moves to undo");
        return;
    }
    Move move = m_history.getTopUndoMove();
    
    int index = move.dstStack->size() - move.cardQuantity;
    std::span<Card*> cards(move.dstStack->data() + index, move.cardQuantity);
    MovingAnimation m(cards, move.dstPos, move.srcPos, [&, move]()
    {
        moveCard(*move.dstStack, *move.srcStack, move.cardQuantity);
        m_history.undo();
    });
    m_movingAnimation.push_back(m);
}

void Freecell::handleInputRedo()
{
    if (m_movingAnimation.size() > 0)
    {
        LOG_WARN("Can't input while card is moving!");
        return;
    }

    if (m_history.isRedoStackEmpty())
    {
        LOG_WARN("No moves to redo");
        return;
    }
    Move move = m_history.getTopRedoMove();

    int index = move.dstStack->size() - move.cardQuantity;
    std::span<Card*> cards(move.dstStack->data() + index, move.cardQuantity);
    MovingAnimation m(cards, move.dstPos, move.srcPos, [&, move]()
    {
        moveCard(*move.dstStack, *move.srcStack, move.cardQuantity);
        m_history.redo();
    });
    m_movingAnimation.push_back(m);
}

void Freecell::handleInputRestart()
{
    if (m_movingAnimation.size() > 0)
    {
        LOG_WARN("Can't input while card is moving!");
        return;
    }

    Dealer::emptyTable(m_board.tableau, m_board.openCells, m_board.foundations);
    m_dealer.fillTableau(m_board.tableau, m_boardMap.tableau);
    m_history.clearStacks();
    updatePlayerData(false, 10000.0f);
    m_currentState = GameState::Playing;
    m_matchData.startTime = Timer::time;
}

void Freecell::handleInputNewGame()
{
    if (m_movingAnimation.size() > 0)
    {
        LOG_WARN("Can't input while card is moving!");
        return;
    }
    
    Dealer::emptyTable(m_board.tableau, m_board.openCells, m_board.foundations);
    m_dealer.shuffleDeck();
    m_dealer.fillTableau(m_board.tableau, m_boardMap.tableau);
    m_history.clearStacks();
    if (m_currentState == GameState::Playing)
        updatePlayerData(false, 10000.0f);
    else
        m_currentState = GameState::Playing;
    m_matchData.startTime = Timer::time;
}

Board& Freecell::board()
{
    return m_board;
}

PlayerData* Freecell::playerData()
{
    return &m_playerData;
}

MatchData* Freecell::matchData()
{
    return &m_matchData;
}

void Freecell::loadPlayerData()
{
    std::filesystem::path file{ "../../resources/gamedata.dat" };
    if (std::filesystem::exists(file))
    {
        Serializer serializer(m_playerData, "../../resources/gamedata.dat");
        serializer.load();
        serializer.deserialize();
        return;
    }

    // first time openning application
    m_playerData.gamesPlayed = 0;
    m_playerData.gamesWon = 0;
    m_playerData.bestTime = 10000.0f;
    
    Serializer serializer(m_playerData, "../../resources/gamedata.dat");
    serializer.serialize();
    serializer.save();
}

void Freecell::updatePlayerData(bool didWon, float time)
{
    m_playerData.gamesPlayed++;
    if (didWon)
        m_playerData.gamesWon++;

    if (time < m_playerData.bestTime)
        m_playerData.bestTime = time;
    
    Serializer serializer(m_playerData, "../../resources/gamedata.dat");
    serializer.serialize();
    serializer.save();
}

void Freecell::setBoardLayout()
{
    float initPosX = 220;
    float offsetX = 120;
    float initPosY = 400;
    float offsetY = 32;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 12; j++)
        {
            m_boardMap.tableau[i][j] = glm::vec2(initPosX + i * offsetX, initPosY - j * offsetY);
        }
    }

    initPosX = 140;
    offsetX = 130;
    for (int i = 0; i < 4; i++)
    {
        m_boardMap.openCells[i] = glm::vec2(initPosX + i * offsetX, 600);
    }

    initPosX += 4 * offsetX + 90;
    for (int i = 0; i < 4; i++)
    {
        m_boardMap.foundations[i] = glm::vec2(initPosX + i * offsetX, 600);
    }
}

void Freecell::createOpenCellsAndFoundations()
{
    for (int i = 0; i < 4; i++)
    {
        m_openCellsBg[i] = Card(-1, -1, 0.125f, 0.875f, glm::vec3(m_boardMap.openCells[i], 0.0f));
        m_foundationsBg[i] = Card(-1, -1, 0.250f, 0.875f, glm::vec3(m_boardMap.foundations[i], 0.0f));
        m_board.openCellsBg[i] = &m_openCellsBg[i];
        m_board.foundationsBg[i] = &m_foundationsBg[i];
    }
}

void Freecell::select(CardStack* stack, int index, bool isDragStart)
{
    if (!isDragStart)
        return;
    
    if ((int) stack->size() == 0)
    {
        LOG_INFO("Cannot select empty stack");
        return;
    }

    if (((int) stack->size()) - 1 != index)
    {
        if (!checkSequence(*stack, index))
        {
            LOG_INFO("Cannot selected unsorted middle card");
            return;
        }
    }

    m_cardSelected.card = (*stack)[index];
    m_cardSelected.stack = stack;
    m_cardSelected.y = index;
    m_cardSelected.pos = m_cardSelected.card->pos;

    std::span<Card*> cards(stack->data() + index, stack->size() - index);
    m_draggingAnimation.start(cards);
}

void Freecell::deselect()
{
    if (m_cardSelected.card == nullptr)
        return;

    m_cardSelected.card = nullptr;
    m_cardSelected.stack = nullptr;
    m_cardSelected.y = -1;

    if (!m_draggingAnimation.isDone())
        m_draggingAnimation.stop();
}

void Freecell::moveBackAndDeselectCard()
{
    if (m_cardSelected.card == nullptr)
        return;
        
    std::span<Card*> cards(m_cardSelected.stack->data() + m_cardSelected.y, m_cardSelected.stack->size() - m_cardSelected.y);
    MovingAnimation m(cards, m_cardSelected.card->pos, m_cardSelected.pos);
    m_movingAnimation.push_back(m);

    deselect();
}

void Freecell::handleClick(CardStack& stack, std::span<glm::vec2> dstAreaPos, int col, int index, bool(Freecell::*isLegalMove)(Card* card, int c), bool isDragStart)
{
    if (m_cardSelected.card == nullptr)
    {
        select(&stack, index, isDragStart);
        return;
    }

    if (!(this->*isLegalMove)(m_cardSelected.card, col))
    {
        LOG_INFO("Invalid move");

        if (glm::vec2(m_cardSelected.card->pos) == m_cardSelected.pos)
        {
            deselect();
            return;
        }

        moveBackAndDeselectCard();
        return;
    }

    glm::vec2 dstPos;
    if (dstAreaPos.size() == 1)
        dstPos = dstAreaPos[0];
    else
        dstPos = dstAreaPos[stack.size()];

    int row = m_cardSelected.y;
    int diff = ((int)m_cardSelected.stack->size()) - row;

    std::span<Card*> cards(m_cardSelected.stack->data() + row, diff);
    CardStack* srcStack = m_cardSelected.stack;
    glm::vec2 srcPos = m_cardSelected.pos;
    MovingAnimation m(cards, m_cardSelected.card->pos, dstPos, [&, diff, srcStack, srcPos, dstPos]()
    {
        moveCard(*srcStack, stack, diff);
        Move m{srcStack, &stack, diff, srcPos, dstPos};
        m_history.recordMove(m);

        if (checkWin())
        {
            LOG_INFO("You Won!");
            m_currentState = GameState::WinAnimation;
        }
    });
    m_movingAnimation.push_back(m);

    deselect();
}

bool Freecell::checkSequence(CardStack& stack, int j)
{
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

bool Freecell::checkWinSequence(CardStack& stack)
{
    int currentCard = ((int)stack.size()) - 1;

    for (int n = currentCard; n > 0; n--)
    {
        bool nextNumber = stack[n]->number <= stack[n - 1]->number;

        if (!nextNumber)
            return false;
    }

    return true;
}

bool Freecell::checkWin()
{
    for (int i = 0; i < 8; i++)
    {
        if (!checkWinSequence(m_board.tableau[i]))
            return false;
    }

    return true;
}

bool Freecell::isComplete()
{
    for (CardStack& stack : m_board.foundations)
    {
        if (stack.size() != 13)
            return false;
    }

    return true;
}

void Freecell::playWinAnimation()
{
    for (CardStack& stack : m_board.openCells)
    {
        if (stack.empty())
            continue;
        winMoves(stack, m_board.foundations, m_boardMap.foundations, &Freecell::foundationsIsLegalMove);
    }

    for (CardStack& stack : m_board.tableau)
    {
        if (stack.empty())
            continue;
        winMoves(stack, m_board.foundations, m_boardMap.foundations, &Freecell::foundationsIsLegalMove);
    }
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
        emptyTableColumns += (int) m_board.tableau[i].empty();
    }

    return emptyTableColumns * emptyOpenCells + emptyOpenCells;
}



int Freecell::getIndexX(int n, double xPos)
{
    for (int i = 0; i < n; i++)
    {
        if (xPos > m_boardMap.tableau[i][0].x - 50 && xPos < m_boardMap.tableau[i][0].x + 50)
            return i;
    }
    return -1;
}

int Freecell::getTopAreaIndexX(std::span<glm::vec2> area, double xPos)
{
    for (int i = 0; i < 4; i++)
    {
        if (xPos > area[i].x - 50 && xPos < area[i].x + 50)
            return i;
    }

    return -1;
}

int Freecell::getIndexY(int n, int col, double yPos)
{
    if (n == 0)
    {
        if (yPos < m_boardMap.tableau[col][0].y + 74 && yPos > m_boardMap.tableau[col][0].y - 74)
            return 0;
        else
            return -1;
    }

    for (int i = 0; i < n - 1; i++)
    {
        if (yPos < m_boardMap.tableau[col][i].y + 74 && yPos > m_boardMap.tableau[col][i].y + 42)
            return i;
    }


    if (yPos < m_boardMap.tableau[col][n - 1].y + 74 && yPos > m_boardMap.tableau[col][n - 1].y - 74)
        return n - 1;

    return -1;
}

void Freecell::winMoves(CardStack& src, std::span<CardStack> dst, std::span<glm::vec2> dstAreaPos, bool(Freecell::*isLegalMove)(Card* card, int c))
{
    for (int i = 0; i < 4; i++)
    {
        if ((this->*isLegalMove)(src.back(), i))
        {
            std::span<Card*> cards(src.data() + src.size() - 1, 1);
            glm::vec3 srcCardPos = cards.back()->pos;
            glm::vec2 dstPos = dstAreaPos[i];
            MovingAnimation m(cards, cards.back()->pos, dstAreaPos[i], [&, dst, i, srcCardPos, dstPos]()
            {
                moveCard(src, dst[i], 1);
                // TODO: check if want to record movements
                Move m{&src, &dst[i], 1, srcCardPos, dstPos};
                m_history.recordMove(m);
            });
            m_movingAnimation.push_back(m);            
        }
    }
}

bool Freecell::tryMoveFromTo(CardStack& src, std::span<CardStack> dst, std::span<glm::vec2> dstAreaPos, bool(Freecell::*isLegalMove)(Card* card, int c))
{
    for (int i = 0; i < 4; i++)
    {
        if ((this->*isLegalMove)(src.back(), i))
        {
            // std::span<Card*> cards(src.data() + src.size() - 1, 1);
            // MovingAnimation m(cards, cards.back()->pos, dstAreaPos[i]);
            // m_board.movingAnimation.push_back(m);

            // moveCard(src, dst[i], 1);
            // m_history.recordMove(&src, &dst[i], 1, cards.back()->pos, dstAreaPos[i]);

            // TODO: check if it's necessary to use on complete for animations
            std::span<Card*> cards(src.data() + src.size() - 1, 1);
            glm::vec3 srcCardPos = cards.back()->pos;
            glm::vec2 dstPos = dstAreaPos[i];
            MovingAnimation m(cards, cards.back()->pos, dstAreaPos[i], [&, dst, i, srcCardPos, dstPos]()
            {
                moveCard(src, dst[i], 1);
                Move m{&src, &dst[i], 1, srcCardPos, dstPos};
                m_history.recordMove(m);

                if (checkWin())
                {
                    LOG_INFO("You Won!");
                    m_currentState = GameState::WinAnimation;
                }
            });
            m_movingAnimation.push_back(m);
            
            return true;
        }
    }

    LOG_INFO("No valid move for open cells or foundations");
    deselect();
    return false;
}

bool Freecell::openCellsIsLegalMove(Card* card, int col)
{
    if (m_cardSelected.card != nullptr)
    {
        if (m_cardSelected.y != ((int) m_cardSelected.stack->size()) - 1)
            return false;
    }

    return m_board.openCells[col].size() == 0;
}

bool Freecell::foundationsIsLegalMove(Card* card, int col)
{
    if (m_cardSelected.card != nullptr)
    {
        if (m_cardSelected.y != ((int) m_cardSelected.stack->size()) - 1)
            return false;
    }

    if (m_board.foundations[col].size() == 0)
        return card->number == 0;

    return m_board.foundations[col].back()->number == card->number - 1
        && m_board.foundations[col].back()->suit == card->suit;
}

bool Freecell::tableIsLegalMove(Card* card, int col)
{
    int cardSelectedRow = m_cardSelected.y;
    int diff = ((int)m_cardSelected.stack->size()) - cardSelectedRow;
    if (getMaxCardsToMove(m_board.tableau[col].empty()) < diff)
        return false;

    if (m_board.tableau[col].size() == 0)
        return true;

    bool diffColor =  m_board.tableau[col].back()->suit % 2 != card->suit % 2;
    bool nextNumber = m_board.tableau[col].back()->number - 1 == card->number;

    return diffColor && nextNumber;   
}

void Freecell::moveCard(CardStack& src, CardStack& dst, int n)
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
