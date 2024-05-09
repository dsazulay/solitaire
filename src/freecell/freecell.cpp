#include "freecell.h"

#include "../event.h"
#include "../dispatcher.h"
#include "../utils/log.h"

Freecell::Freecell()
{
    m_gameLogic.init(&m_boardManager);
}

auto Freecell::init(AnimationEngine* engine) -> void
{
    animationEngine = engine;
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
    if (m_currentState == GameState::Playing)
    {
        m_matchData.currentTime = Timer::time - m_matchData.startTime - m_matchData.timePaused;
    }
    else if (m_currentState == GameState::WinAnimation)
    {
        if (!m_gameLogic.isComplete() &&
                animationEngine->getMovingAnimationQuantity() == 0)
        {
            playWinAnimation();
        }
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
    if (m_currentState != GameState::Playing)
        return;

    if (animationEngine->getMovingAnimationQuantity() > 0)
    {
        LOG_WARN("Can't input while card is moving!");
        return;
    }
    auto cardClicked = m_boardManager.getStackAndPos(xPos, yPos);
    if (!cardClicked.has_value())
    {
        if (!m_boardManager.getCardSelected().has_value())
            return;
        moveBackAndDeselectCard();
        return;
    }

    // TODO: maybe put this insed a trySelect method
    if (!m_boardManager.getCardSelected().has_value())
    {
        if (!isDragStart)
            return;

        int stackSize = static_cast<int>(cardClicked->stack->size());
        if (stackSize == 0)
        {
            LOG_INFO("Cannot select empty stack");
            return;
        }

        if (stackSize - 1 != cardClicked->index)
        {
            if (!m_gameLogic.checkSequence(*cardClicked->stack,
                        cardClicked->index))
            {
                LOG_INFO("Cannot selected unsorted middle card");
                return;
            }
        }
        std::span<CardEntity*> cards(&cardClicked->stack->at(cardClicked->index),
                stackSize - cardClicked->index);
        animationEngine->addDraggingAnimation(DraggingAnimation{ cards });

        m_boardManager.selectCard(cardClicked.value());
        return;
    }

    IsLegalMoveFunc checkMoveFunc = &FreecellGameLogic::openCellsIsLegalMove;
    switch (cardClicked->area)
    {
    case FreecellArea::OpenCells:
        checkMoveFunc = &FreecellGameLogic::openCellsIsLegalMove;
        break;
    case FreecellArea::Foundations:
        checkMoveFunc = &FreecellGameLogic::foundationsIsLegalMove;
        break;
    case FreecellArea::Tableau:
        checkMoveFunc = &FreecellGameLogic::tableauIsLegalMove;
        break;
    }
    handleClick(*cardClicked->stack, cardClicked->pos, checkMoveFunc);
}

auto Freecell::handleInputDoubleClick(double xPos, double yPos) -> void
{
    auto& m_board = m_boardManager.board();
    auto& m_boardMap = m_boardManager.boardMap();
    if (m_currentState != GameState::Playing)
    {
        return;
    }
    if (animationEngine->isAnyAnimationPlaying())
    {
        LOG_WARN("Can't input while card is moving!");
        return;
    }
    auto cardClicked = m_boardManager.getStackAndPos(xPos, yPos);
    if (!cardClicked.has_value())
        return;

    if (cardClicked->area == FreecellArea::Foundations)
    {
        LOG_WARN("No action for double click on foundations");
        return;
    }

    if (static_cast<int>(cardClicked->stack->size()) == 0)
    {
        LOG_WARN("No action for double click an empty stack");
        return;
    }

    if (cardClicked->area == FreecellArea::OpenCells)
    {
        tryMoveFromTo(*cardClicked->stack, m_board.foundations, m_boardMap.foundations, &FreecellGameLogic::foundationsIsLegalMove);
        return;
    }

    if (cardClicked->index < static_cast<int>(cardClicked->stack->size()) - 1)
    {
        LOG_WARN("No action for double click on a middle card of a table stack");
        return;
    }

    if (!tryMoveFromTo(*cardClicked->stack, m_board.foundations, m_boardMap.foundations, &FreecellGameLogic::foundationsIsLegalMove))
    {
        tryMoveFromTo(*cardClicked->stack, m_board.openCells, m_boardMap.openCells, &FreecellGameLogic::openCellsIsLegalMove);
    }
}

auto Freecell::handleInputUndo() -> void
{
    if (m_currentState != GameState::Playing)
    {
        return;
    }
    if (animationEngine->isAnyAnimationPlaying())
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
        m_boardManager.moveCard(*move.dstStack, *move.srcStack, move.cardQuantity);
        m_history.undo();
    });
    animationEngine->addMovingAnimation(m);
}

auto Freecell::handleInputRedo() -> void
{
    if (m_currentState != GameState::Playing)
    {
        return;
    }
    if (animationEngine->isAnyAnimationPlaying())
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
        m_boardManager.moveCard(*move.dstStack, *move.srcStack, move.cardQuantity);
        m_history.redo();
    });
    animationEngine->addMovingAnimation(m);
}

auto Freecell::handleInputRestart() -> void
{
    if (animationEngine->isAnyAnimationPlaying())
    {
        LOG_WARN("Can't input while card is moving!");
        return;
    }
    m_boardManager.emptyTable();
    m_boardManager.fillTable();
    m_boardManager.turnCardsUp();
    m_boardManager.updateCardList();

    m_history.clearStacks();
    m_dataManager.updatePlayerData(false, 0.0);
    m_currentState = GameState::Playing;
    m_matchData.startTime = Timer::time;
    m_matchData.timePaused = 0.0f;
}

auto Freecell::handleInputNewGame() -> void
{
    if (animationEngine->isAnyAnimationPlaying())
    {
        LOG_WARN("Can't input while card is moving!");
        return;
    }
    m_boardManager.emptyTable();
    m_boardManager.shuffleDeck();
    m_boardManager.fillTable();
    m_boardManager.turnCardsUp();
    m_boardManager.updateCardList();

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

    if (animationEngine->isAnyAnimationPlaying())
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


auto Freecell::moveBackAndDeselectCard() -> void
{
    auto card = m_boardManager.getCardSelected().value();
    std::span<CardEntity*> cards(&card.stack->at(card.index),
            card.stack->size() - card.index);
    MovingAnimation m(cards, card.stack->at(card.index)->transform.pos(),
            card.selectionPos);
    animationEngine->addMovingAnimation(m);

    m_boardManager.deselectCard();
    animationEngine->stopDraggingAnimation();
}

auto Freecell::handleClick(CardStack& stack, glm::vec2 dstPos,
        IsLegalMoveFunc isLegalMove) -> void
{
    auto cardSelected = m_boardManager.getCardSelected().value();
    auto card = cardSelected.stack->at(cardSelected.index);

    if (!(&m_gameLogic->*isLegalMove)(card, stack))
    {
        LOG_INFO("Invalid move");

        if (glm::vec2(card->transform.pos()) == cardSelected.selectionPos)
        {
            m_boardManager.deselectCard();
            animationEngine->stopDraggingAnimation();
            return;
        }

        moveBackAndDeselectCard();
        return;
    }

    int row = cardSelected.index;
    int diff = static_cast<int>(cardSelected.stack->size()) - row;

    std::span<CardEntity*> cards(&cardSelected.stack->at(row), diff);
    CardStack* srcStack = cardSelected.stack;
    glm::vec2 srcPos = cardSelected.selectionPos;
    MovingAnimation m(cards, card->transform.pos(), dstPos,
            [&, diff, srcStack, srcPos, dstPos]()
    {
        m_boardManager.moveCard(*srcStack, stack, diff);
        Move m{srcStack, &stack, diff, srcPos, dstPos};
        m_history.recordMove(m);

        if (m_gameLogic.checkWin())
        {
            LOG_INFO("You Won!");
            m_currentState = GameState::WinAnimation;
        }
    });
    animationEngine->addMovingAnimation(m);

    m_boardManager.deselectCard();
    animationEngine->stopDraggingAnimation();
}


auto Freecell::playWinAnimation() -> void
{
    auto& m_board = m_boardManager.board();
    auto& m_boardMap = m_boardManager.boardMap();
    for (CardStack& stack : m_board.openCells)
    {
        if (stack.empty())
            continue;
        winMoves(stack, m_board.foundations, m_boardMap.foundations, &FreecellGameLogic::foundationsIsLegalMove);
    }

    for (CardStack& stack : m_board.tableau)
    {
        if (stack.empty())
            continue;
        winMoves(stack, m_board.foundations, m_boardMap.foundations, &FreecellGameLogic::foundationsIsLegalMove);
    }
}

auto Freecell::winMoves(CardStack& src, std::span<CardStack> dst, std::span<float> dstAreaPos, IsLegalMoveFunc isLegalMove) -> void
{
    for (int i = 0; i < 4; i++)
    {
        if ((&m_gameLogic->*isLegalMove)(src.back(), dst[i]))
        {
            std::span<CardEntity*> cards(&src.at(src.size() - 1), 1);
            glm::vec3 srcCardPos = cards.back()->transform.pos();
            glm::vec2 dstPos = glm::vec2{dstAreaPos[i], SPECIAL_AREAS_Y};
            MovingAnimation m(cards, cards.back()->transform.pos(), dstPos, [&, dst, i, srcCardPos, dstPos]()
            {
                m_boardManager.moveCard(src, dst[i], 1);
                // TODO: check if want to record movements
                Move m{&src, &dst[i], 1, srcCardPos, dstPos};
                m_history.recordMove(m);
            });
            animationEngine->addMovingAnimation(m);
        }
    }
}

auto Freecell::tryMoveFromTo(CardStack& src, std::span<CardStack> dst, std::span<float> dstAreaPos, IsLegalMoveFunc isLegalMove) -> bool
{
    for (int i = 0; i < 4; i++)
    {
        if ((&m_gameLogic->*isLegalMove)(src.back(), dst[i]))
        {
            // TODO: check if it's necessary to use on complete for animations
            std::span<CardEntity*> cards(&src.at(src.size() - 1), 1);
            glm::vec3 srcCardPos = cards.back()->transform.pos();
            glm::vec2 dstPos = glm::vec2{dstAreaPos[i], SPECIAL_AREAS_Y};
            MovingAnimation m(cards, cards.back()->transform.pos(), dstPos, [&, dst, i, srcCardPos, dstPos]()
            {
                m_boardManager.moveCard(src, dst[i], 1);
                Move m{&src, &dst[i], 1, srcCardPos, dstPos};
                m_history.recordMove(m);

                if (m_gameLogic.checkWin())
                {
                    LOG_INFO("You Won!");
                    m_currentState = GameState::WinAnimation;
                }
            });
            animationEngine->addMovingAnimation(m);

            return true;
        }
    }

    LOG_INFO("No valid move for open cells or foundations");

    m_boardManager.deselectCard();
    animationEngine->stopDraggingAnimation();

    return false;
}

