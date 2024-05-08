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
            m_board.cards.push_back(card);
    }

    for (auto stack : m_board.openCells)
    {
        if (stack.size() > 0)
            m_board.cards.push_back(stack.back());
    }

    for (auto stack : m_board.foundations)
    {
        unsigned long size = stack.size();
        if (size > 0)
        {
            if (size > 1)
                m_board.cards.push_back(stack[size - 2]);

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

auto FreecellBoardManager::getArea(double x, double y) -> FreecellArea
{
    // top area
    if (y > SPECIAL_AREAS_Y - CARD_HALF_HEIGHT &&
            y < SPECIAL_AREAS_Y + CARD_HALF_HEIGHT)
    {
        // open cells
        if (x < HALF_SCREEN_WIDTH)
            return FreecellArea::OpenCells;

        return FreecellArea::Foundations;
    }
    return FreecellArea::Tableau;
}

auto FreecellBoardManager::getStackAndPos(double x, double y) ->
    std::optional<CardClicked>
{
    FreecellArea area = getArea(x, y);

    if (area == FreecellArea::OpenCells)
    {
        int index = getIndexX(m_boardMap.openCells, x);
        if (index == -1)
            return std::nullopt;

        CardClicked cardClicked{};
        cardClicked.stack = &m_board.openCells.at(index);
        cardClicked.pos = { m_boardMap.openCells.at(index), SPECIAL_AREAS_Y };
        cardClicked.index = static_cast<int>(cardClicked.stack->size()) - 1;
        cardClicked.area = FreecellArea::OpenCells;
        return std::optional<CardClicked>{ cardClicked };
    }

    if (area == FreecellArea::Foundations)
    {
        int index = getIndexX(m_boardMap.foundations, x);
        if (index == -1)
            return std::nullopt;

        CardClicked cardClicked{};
        cardClicked.stack = &m_board.foundations.at(index);
        cardClicked.pos = { m_boardMap.foundations.at(index), SPECIAL_AREAS_Y };
        cardClicked.index = static_cast<int>(cardClicked.stack->size()) - 1;
        cardClicked.area = FreecellArea::Foundations;
        return std::optional<CardClicked>{ cardClicked };
    }

    if (area == FreecellArea::Tableau)
    {
        int index = getIndexX(m_boardMap.tableauX, x);
        if (index == -1)
            return std::nullopt;

        auto& stack = m_board.tableau.at(index);
        int stackSize = static_cast<int>(stack.size());
        int yIndex = getIndexY(stackSize, y);
        if (yIndex == -1)
            return std::nullopt;

        float yPos = m_boardMap.tableauY.at(yIndex + 1);
        if (stackSize == 0)
            yPos = m_boardMap.tableauY.at(yIndex);

        CardClicked cardClicked{};
        cardClicked.stack = &stack;
        cardClicked.pos = { m_boardMap.tableauX.at(index), yPos };
        cardClicked.index = yIndex;
        cardClicked.area = FreecellArea::Tableau;
        return std::optional<CardClicked>{ cardClicked };
    }

    return std::nullopt;
}

auto FreecellBoardManager::getIndexX(std::span<float> area, double xPos) -> int
{
    for (int i = 0; i < static_cast<int>(area.size()); i++)
    {
        auto x = area[i];
        if (xPos > x - CARD_HALF_WIDTH && xPos < x + CARD_HALF_WIDTH)
            return i;
    }

    return -1;
}

auto FreecellBoardManager::getIndexY(int stackSize, double yPos) -> int
{
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


auto FreecellBoardManager::board() -> FreecellBoard&
{
    return m_board;
}

auto FreecellBoardManager::boardMap() -> FreecellBoardMap&
{
    return m_boardMap;
}


