#include "freecell_boardmanager.h"

auto FreecellBoardManager::createDeck() -> void
{
    m_dealer.createFreecellDeck();
}

auto FreecellBoardManager::shuffleDeck() -> void
{
    m_dealer.shuffleDeck();
}

auto FreecellBoardManager::updateCardList() -> void
{
    m_board.cards.clear();
    for (auto stack : m_board.tableau)
    {
        for (auto card : stack)
        {
            m_board.cards.push_back(card);
        }
    }

    for (auto stack : m_board.openCells)
    {
        if (stack.size() > 0)
        {
            m_board.cards.push_back(stack.back());
        }
    }

    for (auto stack : m_board.foundations)
    {
        unsigned long size = stack.size();
        if (size > 0)
        {
            if (size > 1)
            {
                m_board.cards.push_back(stack[size - 2]);
            }
            m_board.cards.push_back(stack.back());
        }
    }
}

auto FreecellBoardManager::emptyTable() -> void
{
    for (auto& stack : m_board.tableau)
        stack = CardStack();

    for (auto& stack : m_board.openCells)
        stack = CardStack();

    for (auto& stack : m_board.foundations)
        stack = CardStack();
}

auto FreecellBoardManager::fillTable() -> void
{
    constexpr const int reserveSize = 10;
    constexpr const int fullStackSize = 7;

    int tableauSize = static_cast<int>(m_board.tableau.size());
    int tableauHalfSize = tableauSize / 2;
    int index = 0;
    for (int i = 0; i < tableauHalfSize; i++)
    {
        m_board.tableau[i].reserve(reserveSize);
        for (int j = 0; j < fullStackSize; j++)
        {
            m_dealer.deck()[index].transform.pos(glm::vec3(m_boardMap.tableauX[i],
                        m_boardMap.tableauY[j], 0.0));
            m_board.tableau[i].emplace_back(&m_dealer.deck()[index]);
            index++;
        }
    }

    constexpr const int stackSize = 6;
    for (int i = tableauHalfSize; i < tableauSize; i++)
    {
        m_board.tableau[i].reserve(reserveSize);
        for (int j = 0; j < stackSize; j++)
        {
            m_dealer.deck()[index].transform.pos(glm::vec3(m_boardMap.tableauX[i],
                        m_boardMap.tableauY[j], 0.0));
            m_board.tableau[i].emplace_back(&m_dealer.deck()[index]);
            index++;
        }
    }
}

auto FreecellBoardManager::turnCardsUp() -> void
{
    m_dealer.turnCardsUp();
}

auto FreecellBoardManager::turnCardsDown() -> void
{
    m_dealer.turnCardsDown();
}

auto FreecellBoardManager::board() -> FreecellBoard&
{
    return m_board;
}

auto FreecellBoardManager::boardMap() -> FreecellBoardMap&
{
    return m_boardMap;
}

