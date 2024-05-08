#include "freecell.h"

#include "../event.h"
#include "../dispatcher.h"
#include "../utils/log.h"

Freecell::Freecell()
{
    m_inputHandler.init(&m_boardManager);
    m_gameLogic.init(&m_boardManager);
}

auto Freecell::init() -> void
{
    createOpenCellsAndFoundations();
    m_dataManager.loadPlayerData();
    m_boardManager.createDeck();
    m_boardManager.shuffleDeck();
    m_boardManager.fillTable();
    m_boardManager.updateCardList();
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
        if (!m_gameLogic.isComplete() && m_movingAnimation.size() == 0)
            playWinAnimation();
        else if(m_gameLogic.isComplete())
        {
            m_currentState = GameState::Won;
            m_dataManager.updatePlayerData(true, m_matchData.currentTime);
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
    auto& m_board = m_boardManager.board();
    auto& m_boardMap = m_boardManager.boardMap();

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
    if (yPos > SPECIAL_AREAS_Y - CARD_HALF_HEIGHT && yPos < SPECIAL_AREAS_Y + CARD_HALF_HEIGHT)
    {
        // open cells
        if (xPos < HALF_SCREEN_WIDTH)
        {
            int i = getIndexX(m_boardMap.openCells, xPos);
            if (i == -1)
            {
                moveBackAndDeselectCard();
                return;
            }
            glm::vec2 dtsAreaPos{m_boardMap.openCells.at(i), SPECIAL_AREAS_Y};
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
            glm::vec2 dtsAreaPos{m_boardMap.foundations.at(i), SPECIAL_AREAS_Y};
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
    auto& m_board = m_boardManager.board();
    auto& m_boardMap = m_boardManager.boardMap();
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
    if (yPos > SPECIAL_AREAS_Y - CARD_HALF_HEIGHT && yPos < SPECIAL_AREAS_Y + CARD_HALF_HEIGHT)
    {
        // open cells
        if (xPos < HALF_SCREEN_WIDTH)
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
    std::span<CardEntity*> cards(&move.dstStack->at(index), move.cardQuantity);
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
    std::span<CardEntity*> cards(&move.dstStack->at(index), move.cardQuantity);
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

    m_inputHandler.restart();

    m_history.clearStacks();
    m_dataManager.updatePlayerData(false, 0.0);
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

    m_inputHandler.newGame();

    m_history.clearStacks();
    if (m_currentState != GameState::Won)
    {
        m_dataManager.updatePlayerData(false, 0.0);
    }
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
        m_boardManager.turnCardsUp();
        m_boardManager.updateCardList();
    }
    else if (m_currentState == GameState::Playing)
    {
        m_matchData.timePausedStart = Timer::time;
        m_currentState = GameState::Pause;
        m_boardManager.turnCardsDown();
    }
}

auto Freecell::handleInputPrintCards() -> void
{
    auto& m_board = m_boardManager.board();
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

auto Freecell::board() -> FreecellBoard&
{
    return m_boardManager.board();
}

auto Freecell::playerData() -> PlayerData*
{
    return m_dataManager.playerData();
}

auto Freecell::matchData() -> MatchData*
{
    return &m_matchData;
}

auto Freecell::createOpenCellsAndFoundations() -> void
{
    auto& m_board = m_boardManager.board();
    auto& m_boardMap = m_boardManager.boardMap();
    constexpr const glm::vec2 openCellsUV = { 0.125, 0.875 };
    constexpr const glm::vec2 foundationsUV = { 0.250f, 0.875 };

    for (int i = 0; i < SPECIAL_AREAS_SIZE; i++)
    {
        glm::vec3 openCellsPos{m_boardMap.openCells[i], SPECIAL_AREAS_Y, 0.0f};
        glm::vec3 foundationsPos{m_boardMap.foundations[i], SPECIAL_AREAS_Y, 0.0f};
        m_specialAreas[i] = CardBg(openCellsUV, openCellsPos);
        m_specialAreas[i + SPECIAL_AREAS_SIZE] = CardBg(foundationsUV,
                foundationsPos);
    }

    m_board.cardBgs = m_specialAreas;
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
        if (!m_gameLogic.checkSequence(*stack, index))
        {
            LOG_INFO("Cannot selected unsorted middle card");
            return;
        }
    }

    m_cardSelected.card = (*stack)[index];
    m_cardSelected.stack = stack;
    m_cardSelected.y = index;
    m_cardSelected.pos = m_cardSelected.card->transform.pos();

    std::span<CardEntity*> cards(&stack->at(index), stack->size() - index);
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

    std::span<CardEntity*> cards(&m_cardSelected.stack->at(m_cardSelected.y),
            m_cardSelected.stack->size() - m_cardSelected.y);
    MovingAnimation m(cards, m_cardSelected.card->transform.pos(),
            m_cardSelected.pos);
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

        if (glm::vec2(m_cardSelected.card->transform.pos()) == m_cardSelected.pos)
        {
            deselect();
            return;
        }

        moveBackAndDeselectCard();
        return;
    }

    int row = m_cardSelected.y;
    int diff = static_cast<int>(m_cardSelected.stack->size()) - row;

    std::span<CardEntity*> cards(&m_cardSelected.stack->at(row), diff);
    CardStack* srcStack = m_cardSelected.stack;
    glm::vec2 srcPos = m_cardSelected.pos;
    MovingAnimation m(cards, m_cardSelected.card->transform.pos(), dstPos, [&, diff, srcStack, srcPos, dstPos]()
    {
        moveCard(*srcStack, stack, diff);
        Move m{srcStack, &stack, diff, srcPos, dstPos};
        m_history.recordMove(m);

        if (m_gameLogic.checkWin())
        {
            LOG_INFO("You Won!");
            m_currentState = GameState::WinAnimation;
        }
    });
    m_movingAnimation.push_back(m);

    deselect();
}


auto Freecell::playWinAnimation() -> void
{
    auto& m_board = m_boardManager.board();
    auto& m_boardMap = m_boardManager.boardMap();
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


auto Freecell::getIndexX(std::span<float> area, double xPos) -> int
{
    for (int i = 0; i < static_cast<int>(area.size()); i++)
    {
        auto x = area[i];
        if (xPos > x - CARD_HALF_WIDTH && xPos < x + CARD_HALF_WIDTH)
            return i;
    }

    return -1;
}

auto Freecell::getIndexY(int stackSize, double yPos) -> int
{
    auto& m_boardMap = m_boardManager.boardMap();
    if (stackSize == 0)
    {
        auto y = m_boardMap.tableauY.front();
        if (yPos < y + CARD_HALF_HEIGHT && yPos > y - CARD_HALF_HEIGHT)
            return 0;
        else
            return -1;
    }

    for (int i = 0; i < stackSize - 1; i++)
    {
        auto y = m_boardMap.tableauY.at(i);
        if (yPos < y + CARD_HALF_HEIGHT && yPos > y + CARD_MIDDLE_HEIGHT)
            return i;
    }

    auto y = m_boardMap.tableauY.at(stackSize - 1);
    if (yPos < y + CARD_HALF_HEIGHT && yPos > y - CARD_HALF_HEIGHT)
        return stackSize - 1;

    return -1;
}

auto Freecell::winMoves(CardStack& src, std::span<CardStack> dst, std::span<float> dstAreaPos, IsLegalMoveFunc isLegalMove) -> void
{
    for (int i = 0; i < 4; i++)
    {
        if ((this->*isLegalMove)(src.back(), dst[i]))
        {
            std::span<CardEntity*> cards(&src.at(src.size() - 1), 1);
            glm::vec3 srcCardPos = cards.back()->transform.pos();
            glm::vec2 dstPos = glm::vec2{dstAreaPos[i], SPECIAL_AREAS_Y};
            MovingAnimation m(cards, cards.back()->transform.pos(), dstPos, [&, dst, i, srcCardPos, dstPos]()
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
            // TODO: check if it's necessary to use on complete for animations
            std::span<CardEntity*> cards(&src.at(src.size() - 1), 1);
            glm::vec3 srcCardPos = cards.back()->transform.pos();
            glm::vec2 dstPos = glm::vec2{dstAreaPos[i], SPECIAL_AREAS_Y};
            MovingAnimation m(cards, cards.back()->transform.pos(), dstPos, [&, dst, i, srcCardPos, dstPos]()
            {
                moveCard(src, dst[i], 1);
                Move m{&src, &dst[i], 1, srcCardPos, dstPos};
                m_history.recordMove(m);

                if (m_gameLogic.checkWin())
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

auto Freecell::openCellsIsLegalMove(CardEntity* card, const CardStack& stack) -> bool
{
    if (m_cardSelected.card != nullptr)
    {
        if (m_cardSelected.y != static_cast<int>(m_cardSelected.stack->size()) - 1)
            return false;
    }

    return stack.size() == 0;
}

auto Freecell::foundationsIsLegalMove(CardEntity* card, const CardStack& stack) -> bool
{
    if (m_cardSelected.card != nullptr)
    {
        if (m_cardSelected.y != static_cast<int>(m_cardSelected.stack->size()) - 1)
            return false;
    }

    if (stack.size() == 0)
        return card->card.number == 0;

    return stack.back()->card.number == card->card.number - 1
        && stack.back()->card.suit == card->card.suit;
}

auto Freecell::tableIsLegalMove(CardEntity* card, const CardStack& stack) -> bool
{
    int cardSelectedRow = m_cardSelected.y;
    int diff = static_cast<int>(m_cardSelected.stack->size()) - cardSelectedRow;
    if (m_gameLogic.getMaxCardsToMove(stack.empty()) < diff)
        return false;

    if (stack.size() == 0)
        return true;

    bool diffColor =  stack.back()->card.suit % 2 != card->card.suit % 2;
    bool nextNumber = stack.back()->card.number - 1 == card->card.number;

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

    // Update card order after moving card
    m_boardManager.updateCardList();
}
