#include "freecell.h"

#include <filesystem>

#include "event.h"
#include "dispatcher.h"
#include "utils/log.h"
#include "serializer.h"


auto Freecell::init() -> void
{
    loadPlayerData();
    setBoardLayout();
    createOpenCellsAndFoundations();
    m_dealer.shuffleDeck();
    m_dealer.fillTableau(m_board.tableau, m_boardMap.tableauX, m_boardMap.tableauY);

    m_currentState = GameState::Playing;
    m_matchData.startTime = Timer::time;
    m_matchData.timePaused = 0.0f;
}

auto Freecell::update() -> void
{
    auto it = m_movingAnimation.begin();
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
        m_matchData.currentTime = Timer::time - m_matchData.startTime - m_matchData.timePaused;
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

auto Freecell::handleInputClick(double xPos, double yPos, bool isDraging, bool isDragStart) -> void
{
    if (m_currentState != GameState::Playing)
    {
        return;
    }

    if (m_movingAnimation.size() > 0)
    {
        LOG_WARN("Can't input while card is moving!");
        return;
    }

    // top area
    if (yPos > BoardMap::topAreaYPos - BoardMap::cardHalfHeight && yPos < BoardMap::topAreaYPos + BoardMap::cardHalfHeight)
    {
        // open cells
        if (xPos < BoardMap::halfScreenWidth)
        {
            int i = getIndexX(m_boardMap.openCells, xPos);
            if (i == -1)
            {
                moveBackAndDeselectCard();
                return;
            }
            glm::vec2 dtsAreaPos{m_boardMap.openCells.at(i), BoardMap::topAreaYPos};
            auto& stack = m_board.openCells.at(i);
            handleClick(stack, dtsAreaPos, i, static_cast<int>(stack.size()) - 1, &Freecell::openCellsIsLegalMove, isDragStart);
        }
        else
        {
            int i = getIndexX(m_boardMap.foundations, xPos);
            if (i == -1)
            {
                moveBackAndDeselectCard();
                return;
            }
            glm::vec2 dtsAreaPos{m_boardMap.foundations.at(i), BoardMap::topAreaYPos};
            auto& stack = m_board.foundations.at(i);
            handleClick(stack, dtsAreaPos, i, static_cast<int>(stack.size()) - 1, &Freecell::foundationsIsLegalMove, isDragStart);
        }
    }
    else
    {
        int i = getIndexX(m_boardMap.tableauX, xPos);
        if (i == -1)
        {
            moveBackAndDeselectCard();
            return;
        }
        
        auto& stack = m_board.tableau.at(i);
        int stackSize = static_cast<int>(stack.size());
        int j = getIndexY(stackSize, yPos);
        if (j == -1)
        {
            moveBackAndDeselectCard();
            return;
        }

        auto y = m_boardMap.tableauY.at(j + 1);
        if (stackSize == 0)
            y = m_boardMap.tableauY.at(j);

        glm::vec2 dtsAreaPos{m_boardMap.tableauX.at(i), y};
        handleClick(stack, dtsAreaPos, i, j, &Freecell::tableIsLegalMove, isDragStart);
    }
}

auto Freecell::handleInputDoubleClick(double xPos, double yPos) -> void
{
    if (m_currentState != GameState::Playing)
    {
        return;
    }

    if (m_movingAnimation.size() > 0 || !m_draggingAnimation.isDone())
    {
        LOG_WARN("Can't input while card is moving!");
        return;
    }
    // top area
    if (yPos > BoardMap::topAreaYPos - BoardMap::cardHalfHeight && yPos < BoardMap::topAreaYPos + BoardMap::cardHalfHeight)
    {
        // open cells
        if (xPos < BoardMap::halfScreenWidth)
        {
            int i = getIndexX(m_boardMap.openCells, xPos);
            if (i == -1)
                return;

            auto& stack = m_board.openCells.at(i);
            if (static_cast<int>(stack.size()) == 0)
            {
                LOG_WARN("No action for double click an empty open cells stack");
                return;
            }

            tryMoveFromTo(stack, m_board.foundations, m_boardMap.foundations, &Freecell::foundationsIsLegalMove);
        }
        // foundations
        else
        {
            LOG_WARN("No action for double click on foundations");
        }
    }
    else
    {
        int i = getIndexX(m_boardMap.tableauX, xPos);
        if (i == -1)
            return;

        auto& stack = m_board.tableau.at(i);
        int stackSize = static_cast<int>(stack.size());
        int j = getIndexY(stackSize, yPos);
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

        if (!tryMoveFromTo(stack, m_board.foundations, m_boardMap.foundations, &Freecell::foundationsIsLegalMove))
        {
            tryMoveFromTo(stack, m_board.openCells, m_boardMap.openCells, &Freecell::openCellsIsLegalMove);
        }
    }
}

auto Freecell::handleInputUndo() -> void
{
    if (m_currentState != GameState::Playing)
    {
        return;
    }

    if (m_movingAnimation.size() > 0 || !m_draggingAnimation.isDone())
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
    
    int index = static_cast<int>(move.dstStack->size()) - move.cardQuantity;
    std::span<Card*> cards(&move.dstStack->at(index), move.cardQuantity);
    MovingAnimation m(cards, move.dstPos, move.srcPos, [&, move]()
    {
        moveCard(*move.dstStack, *move.srcStack, move.cardQuantity);
        m_history.undo();
    });
    m_movingAnimation.push_back(m);
}

auto Freecell::handleInputRedo() -> void
{
    if (m_currentState != GameState::Playing)
    {
        return;
    }

    if (m_movingAnimation.size() > 0 || !m_draggingAnimation.isDone())
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

    int index = static_cast<int>(move.dstStack->size()) - move.cardQuantity;
    std::span<Card*> cards(&move.dstStack->at(index), move.cardQuantity);
    MovingAnimation m(cards, move.dstPos, move.srcPos, [&, move]()
    {
        moveCard(*move.dstStack, *move.srcStack, move.cardQuantity);
        m_history.redo();
    });
    m_movingAnimation.push_back(m);
}

auto Freecell::handleInputRestart() -> void
{
    if (m_movingAnimation.size() > 0 || !m_draggingAnimation.isDone())
    {
        LOG_WARN("Can't input while card is moving!");
        return;
    }

    m_dealer.emptyTable(m_board.tableau, m_board.openCells, m_board.foundations);
    m_dealer.fillTableau(m_board.tableau, m_boardMap.tableauX, m_boardMap.tableauY);
    m_dealer.turnCardsUp();
    m_history.clearStacks();
    constexpr static float maxTime = 10000.0f;
    updatePlayerData(false, maxTime);
    m_currentState = GameState::Playing;
    m_matchData.startTime = Timer::time;
    m_matchData.timePaused = 0.0f;
}

auto Freecell::handleInputNewGame() -> void
{
    if (m_movingAnimation.size() > 0 || !m_draggingAnimation.isDone())
    {
        LOG_WARN("Can't input while card is moving!");
        return;
    }
    
    m_dealer.emptyTable(m_board.tableau, m_board.openCells, m_board.foundations);
    m_dealer.shuffleDeck();
    m_dealer.fillTableau(m_board.tableau, m_boardMap.tableauX, m_boardMap.tableauY);
    m_dealer.turnCardsUp();
    m_history.clearStacks();
    constexpr static float maxTime = 10000.0f;
    if (m_currentState == GameState::Playing)
        updatePlayerData(false, maxTime);
    else
        m_currentState = GameState::Playing;
    m_matchData.startTime = Timer::time;
    m_matchData.timePaused = 0.0f;
}

auto Freecell::handleInputPause() -> void
{
    if (m_movingAnimation.size() > 0 || !m_draggingAnimation.isDone())
    {
        LOG_WARN("Can't input while card is moving!");
        return;
    }

    if (m_currentState == GameState::Pause)
    {
        m_matchData.timePaused += Timer::time - m_matchData.timePausedStart;
        m_currentState = GameState::Playing;
        m_dealer.turnCardsUp();
    }
    else if (m_currentState == GameState::Playing)
    {
        m_matchData.timePausedStart = Timer::time;
        m_currentState = GameState::Pause;
        m_dealer.turnCardsDown();
    }
}

auto Freecell::handleInputPrintCards() -> void
{
    for (auto& stack : m_board.tableau)
    {
        for (auto c : stack)
        {
            c->print();
        }
        fmt::print("\n");
    }
    fmt::print("\n");
}

auto Freecell::board() -> Board&
{
    return m_board;
}

auto Freecell::playerData() -> PlayerData*
{
    return &m_playerData;
}

auto Freecell::matchData() -> MatchData*
{
    return &m_matchData;
}

auto Freecell::loadPlayerData() -> void
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
    constexpr static float maxTime = 10000.0f;
    m_playerData.gamesPlayed = 0;
    m_playerData.gamesWon = 0;
    m_playerData.bestTime = maxTime;
    
    Serializer serializer(m_playerData, "../../resources/gamedata.dat");
    serializer.serialize();
    serializer.save();
}

auto Freecell::updatePlayerData(bool didWon, float time) -> void
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

auto Freecell::setBoardLayout() -> void
{
    constexpr static float tableauInitPosX = 220.0f;
    constexpr static float tableOffsetX = 120.0f;
    constexpr static float tableauInitPosY = 380.0f;
    constexpr static float tableauOffsetY = 30.0f;

    for (int i = 0; i < Board::tableauSize; i++)
    {
        m_boardMap.tableauX[i] = tableauInitPosX + (float) i * tableOffsetX;
    }

    for (int i = 0; i < Board::stackMaxSize; i++)
    {
        m_boardMap.tableauY[i] = tableauInitPosY - (float) i * tableauOffsetY;
    }

    constexpr static float openCellsInitPosX = 140.0f;
    constexpr static float openCellsFoundationsOffsetX = 130.0f;
    constexpr static float foundationsInitPosX = openCellsInitPosX + 90.f + 4 * openCellsFoundationsOffsetX;

    for (int i = 0; i < Board::openCellsAndFoundSize; i++)
    {
        auto offset = (float) i * openCellsFoundationsOffsetX;
        m_boardMap.openCells[i] = openCellsInitPosX + offset;
        m_boardMap.foundations[i] = foundationsInitPosX + offset;
    }
}

auto Freecell::createOpenCellsAndFoundations() -> void
{
    constexpr static float openCellsUVX = 0.125f;
    constexpr static float foundationsUVX = 0.250f;
    constexpr static float openCellsFoundUVY = 0.875f;

    for (int i = 0; i < Board::openCellsAndFoundSize; i++)
    {
        glm::vec3 openCellsPos{m_boardMap.openCells[i], BoardMap::topAreaYPos, 0.0f};
        glm::vec3 foundationsPos{m_boardMap.foundations[i], BoardMap::topAreaYPos, 0.0f};
        m_openCellsBg[i] = Card(-1, -1, openCellsUVX, openCellsFoundUVY, 0.0f, 0.0f, openCellsPos);
        m_foundationsBg[i] = Card(-1, -1, foundationsUVX, openCellsFoundUVY, 0.0f, 0.0f, foundationsPos); 
        m_board.openCellsBg[i] = &m_openCellsBg[i];
        m_board.foundationsBg[i] = &m_foundationsBg[i];
    }
}

auto Freecell::select(CardStack* stack, int index, bool isDragStart) -> void
{
    if (!isDragStart)
        return;
    
    if (static_cast<int>(stack->size()) == 0)
    {
        LOG_INFO("Cannot select empty stack");
        return;
    }

    if (static_cast<int>(stack->size()) - 1 != index)
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

    std::span<Card*> cards(&stack->at(index), stack->size() - index);
    m_draggingAnimation.start(cards);
}

auto Freecell::deselect() -> void
{
    if (m_cardSelected.card == nullptr)
        return;

    m_cardSelected.card = nullptr;
    m_cardSelected.stack = nullptr;
    m_cardSelected.y = -1;

    if (!m_draggingAnimation.isDone())
        m_draggingAnimation.stop();
}

auto Freecell::moveBackAndDeselectCard() -> void
{
    if (m_cardSelected.card == nullptr)
        return;
        
    std::span<Card*> cards(&m_cardSelected.stack->at(m_cardSelected.y), m_cardSelected.stack->size() - m_cardSelected.y);
    MovingAnimation m(cards, m_cardSelected.card->pos, m_cardSelected.pos);
    m_movingAnimation.push_back(m);

    deselect();
}

auto Freecell::handleClick(CardStack& stack, glm::vec2 dstPos, int col, int index, IsLegalMoveFunc isLegalMove, bool isDragStart) -> void
{ 
    if (m_cardSelected.card == nullptr)
    {
        select(&stack, index, isDragStart);
        return;
    }

    if (!(this->*isLegalMove)(m_cardSelected.card, stack))
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

    int row = m_cardSelected.y;
    int diff = static_cast<int>(m_cardSelected.stack->size()) - row;

    std::span<Card*> cards(&m_cardSelected.stack->at(row), diff);
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

auto Freecell::checkSequence(const CardStack& stack, int j) -> bool
{
    int currentCard = static_cast<int>(stack.size()) - 1;

    for (int n = currentCard; n > j; n--)
    {
        bool diffColor = stack[n]->suit % 2 != stack[n - 1]->suit % 2;
        bool nextNumber = stack[n]->number == stack[n - 1]->number - 1;

        if (!(diffColor && nextNumber))
            return false;
    }

    return true;
}

auto Freecell::checkWinSequence(const CardStack& stack) -> bool
{
    int currentCard = static_cast<int>(stack.size()) - 1;

    for (int n = currentCard; n > 0; n--)
    {
        bool nextNumber = stack[n]->number <= stack[n - 1]->number;

        if (!nextNumber)
            return false;
    }

    return true;
}

auto Freecell::checkWin() -> bool
{
    for (const auto& stack : m_board.tableau)
    {
        if (!checkWinSequence(stack))
            return false;
    }

    return true;
}

auto Freecell::isComplete() -> bool
{
    constexpr static int winStackSize = 13;
    for (CardStack& stack : m_board.foundations)
    {
        if (stack.size() != winStackSize)
            return false;
    }

    return true;
}

auto Freecell::playWinAnimation() -> void
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

auto Freecell::getMaxCardsToMove(bool movingToEmptySpace) -> int
{
    // Initialize with one because you can always move one card
    int emptyOpenCells = 1;
    for (auto stack : m_board.openCells)
    {
        emptyOpenCells += static_cast<int>(stack.empty());
    }

    int emptyTableColumns = movingToEmptySpace ? -1 : 0;
    for (auto stack : m_board.tableau)
    {
        emptyTableColumns += static_cast<int>(stack.empty());
    }

    return emptyTableColumns * emptyOpenCells + emptyOpenCells;
}

auto Freecell::getIndexX(std::span<float> area, double xPos) -> int
{
    for (int i = 0; i < static_cast<int>(area.size()); i++)
    {
        auto x = area[i];
        if (xPos > x - BoardMap::cardHalfWidth && xPos < x + BoardMap::cardHalfWidth)
            return i;
    }

    return -1;
}

auto Freecell::getIndexY(int stackSize, double yPos) -> int
{
    if (stackSize == 0)
    {
        auto y = m_boardMap.tableauY.front();
        if (yPos < y + BoardMap::cardHalfHeight && yPos > y - BoardMap::cardHalfHeight)
            return 0;
        else
            return -1;
    }

    for (int i = 0; i < stackSize - 1; i++)
    {
        auto y = m_boardMap.tableauY.at(i);
        if (yPos < y + BoardMap::cardHalfHeight && yPos > y + BoardMap::cardMiddleHeight)
            return i;
    }

    auto y = m_boardMap.tableauY.at(stackSize - 1);
    if (yPos < y + BoardMap::cardHalfHeight && yPos > y - BoardMap::cardHalfHeight)
        return stackSize - 1;

    return -1;
}

auto Freecell::winMoves(CardStack& src, std::span<CardStack> dst, std::span<float> dstAreaPos, IsLegalMoveFunc isLegalMove) -> void
{
    for (int i = 0; i < 4; i++)
    {
        if ((this->*isLegalMove)(src.back(), dst[i]))
        {
            std::span<Card*> cards(&src.at(src.size() - 1), 1);
            glm::vec3 srcCardPos = cards.back()->pos;
            glm::vec2 dstPos = glm::vec2{dstAreaPos[i], BoardMap::topAreaYPos};
            MovingAnimation m(cards, cards.back()->pos, dstPos, [&, dst, i, srcCardPos, dstPos]()
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

auto Freecell::tryMoveFromTo(CardStack& src, std::span<CardStack> dst, std::span<float> dstAreaPos, IsLegalMoveFunc isLegalMove) -> bool
{
    for (int i = 0; i < 4; i++)
    {
        if ((this->*isLegalMove)(src.back(), dst[i]))
        {
            // std::span<Card*> cards(src.data() + src.size() - 1, 1);
            // MovingAnimation m(cards, cards.back()->pos, dstAreaPos[i]);
            // m_board.movingAnimation.push_back(m);

            // moveCard(src, dst[i], 1);
            // m_history.recordMove(&src, &dst[i], 1, cards.back()->pos, dstAreaPos[i]);

            // TODO: check if it's necessary to use on complete for animations
            std::span<Card*> cards(&src.at(src.size() - 1), 1);
            glm::vec3 srcCardPos = cards.back()->pos;
            glm::vec2 dstPos = glm::vec2{dstAreaPos[i], BoardMap::topAreaYPos};
            MovingAnimation m(cards, cards.back()->pos, dstPos, [&, dst, i, srcCardPos, dstPos]()
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

auto Freecell::openCellsIsLegalMove(Card* card, const CardStack& stack) -> bool
{
    if (m_cardSelected.card != nullptr)
    {
        if (m_cardSelected.y != static_cast<int>(m_cardSelected.stack->size()) - 1)
            return false;
    }

    return stack.size() == 0;
}

auto Freecell::foundationsIsLegalMove(Card* card, const CardStack& stack) -> bool
{
    if (m_cardSelected.card != nullptr)
    {
        if (m_cardSelected.y != static_cast<int>(m_cardSelected.stack->size()) - 1)
            return false;
    }

    if (stack.size() == 0)
        return card->number == 0;

    return stack.back()->number == card->number - 1
        && stack.back()->suit == card->suit;
}

auto Freecell::tableIsLegalMove(Card* card, const CardStack& stack) -> bool
{
    int cardSelectedRow = m_cardSelected.y;
    int diff = static_cast<int>(m_cardSelected.stack->size()) - cardSelectedRow;
    if (getMaxCardsToMove(stack.empty()) < diff)
        return false;

    if (stack.size() == 0)
        return true;

    bool diffColor =  stack.back()->suit % 2 != card->suit % 2;
    bool nextNumber = stack.back()->number - 1 == card->number;

    return diffColor && nextNumber;   
}

auto Freecell::moveCard(CardStack& src, CardStack& dst, int n) -> void
{
    for (int i = static_cast<int>(src.size()) - n; i < static_cast<int>(src.size()); i++)
    {
        dst.push_back(src[i]);
    }

    for (int i = 0; i < n; i++)
    {
        src.pop_back();
    }
}
