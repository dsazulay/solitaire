#include "freecell_input.h"

#include "../utils/log.h"

auto FreecellInputHandler::restart() -> void
{
    // TODO: move this to animation manager
    //if (m_movingAnimation.size() > 0 || !m_draggingAnimation.isDone())
    //{
    //    LOG_WARN("Can't input while card is moving!");
    //    return;
    //}

    boardManager->emptyTable();
    boardManager->fillTable();
    boardManager->turnCardsUp();
    boardManager->updateCardList();
    LOG_WARN("Estive aqui");
    //m_history.clearStacks();
    //constexpr static float maxTime = 10000.0f;
    //updatePlayerData(false, maxTime);
    //m_currentState = GameState::Playing;
    //m_matchData.startTime = Timer::time;
    //m_matchData.timePaused = 0.0f;
}
/*
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
    m_board.updateCardList();
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
        m_board.updateCardList();
    }
    else if (m_currentState == GameState::Playing)
    {
        m_matchData.timePausedStart = Timer::time;
        m_currentState = GameState::Pause;
        m_dealer.turnCardsDown();
    }
}
*/
