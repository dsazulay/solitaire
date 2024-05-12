#include "scoundrel_boardmanager.h"

auto ScoundrelBoardManager::createDeck() -> void
{
    m_dealer.createScoundrelDeck();
}

auto ScoundrelBoardManager::shuffleDeck() -> void
{
    m_dealer.shuffleDeck();
}

auto ScoundrelBoardManager::updateCardList() -> void
{
    m_board.cards.clear();
    if (m_board.discard.size() > 0)
    {
        m_board.cards.push_back(m_board.discard.back());
    }
    for (auto stack : m_board.room)
    {
        if (stack.size() > 0)
        {
            m_board.cards.push_back(stack.back());
        }
    }
    for (auto card : m_board.weapon)
    {
        m_board.cards.push_back(card);
    }
    if (m_board.potion.size() > 0)
    {
        m_board.cards.push_back(m_board.potion.back());
    }
    if (m_board.hands.size() > 0)
    {
        m_board.cards.push_back(m_board.hands.back());
    }
}

auto ScoundrelBoardManager::emptyTable() -> void
{
    m_board.dungeon = CardStack();
    m_board.discard = CardStack();
    m_board.weapon = CardStack();

    for (auto& stack : m_board.room)
        stack = CardStack();
}

auto ScoundrelBoardManager::fillTable() -> void
{
    m_board.dungeon.reserve(m_dealer.deck().size());
    for (auto& card : m_dealer.deck())
    {
        card.transform.pos(glm::vec3{ m_boardMap.dungeon, 0.0 });
        m_board.dungeon.emplace_back(&card);
    }
}

auto ScoundrelBoardManager::fillRoom() -> void
{
    int startIndex = 3;
    // move remaining card to last position
    // TODO: use iterators until last pos - 1
    for (auto& stack : m_board.room)
    {
        if (stack.size() > 0)
        {
            startIndex = 2;

            if (stack == m_board.room[3])
                break;
            stack.back()->transform.pos(glm::vec3{ m_boardMap.room[3], 0.0 });
            m_board.room[3].push_back(stack.back());
            stack.pop_back();
            break;
        }
    }

    for (int i = startIndex; i >= 0; --i)
    {
        m_board.room[i].push_back(m_board.dungeon.back());
        m_board.dungeon.pop_back();
        m_board.room[i].back()->transform.pos(
            glm::vec3{ m_boardMap.room[i], 0.0 });
    }
}

auto ScoundrelBoardManager::getNumberOfAvailableRooms() -> int
{
    int n = 0;
    for (auto r : m_board.room)
    {
        if (r.size() > 0)
            ++n;
    }
    return n;
}

auto ScoundrelBoardManager::clearTableForNextFloor() -> void
{
    for (auto c : m_board.potion)
        c->transform.pos(m_boardMap.discard);
    moveCard(m_board.potion, m_board.discard, m_board.potion.size());

    for (auto c : m_board.hands)
        c->transform.pos(m_boardMap.discard);
    moveCard(m_board.hands, m_board.discard, m_board.hands.size());
}

auto ScoundrelBoardManager::discardWeapon() -> void
{
    moveCard(m_board.weapon, m_board.discard, m_board.weapon.size());
}

auto ScoundrelBoardManager::run() -> void
{
    // TODO: implement me
}

auto ScoundrelBoardManager::moveCard(CardStack& src, CardStack& dst, int n) -> void
{
    auto it = src.begin() + static_cast<int>(src.size()) - n;
    for (; it < src.end(); ++it)
        dst.push_back(*it);

    for (int i = 0; i < n; i++)
        src.pop_back();

    // Update card order after moving card
    updateCardList();
}

auto ScoundrelBoardManager::getArea(double x, double y) -> ScoundrelArea
{
    if (y > DUNGEON_POS.y - CARD_HALF_HEIGHT &&
            y < DUNGEON_POS.y + CARD_HALF_HEIGHT)
    {
        if (x > DUNGEON_POS.x - CARD_HALF_WIDTH &&
                x < DUNGEON_POS.x + CARD_HALF_WIDTH)
        {
            return ScoundrelArea::Dungeon;
        }
        if (x > DUNGEON_POS.x - CARD_HALF_WIDTH &&
                x < DUNGEON_POS.x + CARD_HALF_WIDTH)
        {
            return ScoundrelArea::Discard;
        }
    }
    else if (y > ROOM_POS.y - CARD_HALF_HEIGHT &&
            y < ROOM_POS.y + CARD_HALF_HEIGHT)
    {
        return ScoundrelArea::Room;
    }
    else if (y > HANDS_POS.y - CARD_HALF_HEIGHT &&
            y < HANDS_POS.y + CARD_HALF_HEIGHT)
    {
        if (x > HANDS_POS.x - CARD_HALF_WIDTH &&
                x < HANDS_POS.x + CARD_HALF_WIDTH)
        {
            return ScoundrelArea::Hands;
        }
        if (x > POTION_POS.x - CARD_HALF_WIDTH &&
                x < POTION_POS.x + CARD_HALF_WIDTH)
        {
            return ScoundrelArea::Potion;
        }

        return ScoundrelArea::Weapon;
    }

    return ScoundrelArea::None;
}

auto ScoundrelBoardManager::getStackAndPos(double x, double y) ->
    std::optional<CardClickedScoundrel>
{
    ScoundrelArea area = getArea(x, y);

    if (area == ScoundrelArea::Room)
    {
        int index = getIndexX(m_boardMap.room, x);
        if (index == -1)
            return std::nullopt;

        auto* stack = &m_board.room.at(index);
        std::optional<CardClickedScoundrel> cardClicked{{
            stack,
            m_boardMap.room.at(index),
            { 0.0, 0.0 },
            ScoundrelArea::Room
        }};

        return cardClicked;
    }

    if (area == ScoundrelArea::Potion)
    {
        std::optional<CardClickedScoundrel> cardClicked{{
            &m_board.potion,
            m_boardMap.potion,
            { 0.0, 0.0 },
            ScoundrelArea::Potion
        }};
        return  cardClicked;
    }

    if (area == ScoundrelArea::Hands)
    {
        std::optional<CardClickedScoundrel> cardClicked{{
            &m_board.hands,
            m_boardMap.hands,
            { 0.0, 0.0 },
            ScoundrelArea::Hands
        }};
        return cardClicked;
    }

    if (area == ScoundrelArea::Weapon)
    {
        std::optional<CardClickedScoundrel> cardClicked{{
            &m_board.weapon,
            m_boardMap.weapon.at(m_board.weapon.size()),
            { 0.0, 0.0 },
            ScoundrelArea::Weapon
        }};
        return cardClicked;
    }

    if (area == ScoundrelArea::Dungeon)
    {
        std::optional<CardClickedScoundrel> cardClicked{{
            &m_board.dungeon,
            m_boardMap.dungeon,
            { 0.0, 0.0 },
            ScoundrelArea::Dungeon
        }};
        return cardClicked;
    }

    return std::nullopt;
}

auto ScoundrelBoardManager::getIndexX(std::span<glm::vec2> area, double xPos) -> int
{
    for (int i = 0; i < static_cast<int>(area.size()); i++)
    {
        auto x = area[i].x;
        if (xPos > x - CARD_HALF_WIDTH && xPos < x + CARD_HALF_WIDTH)
            return i;
    }

    return -1;
}

auto ScoundrelBoardManager::getIndexY(int stackSize, double yPos) -> int
{
  /*  if (stackSize == 0)
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
*/
    return -1;
}

auto ScoundrelBoardManager::selectCard(CardClickedScoundrel c) -> void
{
    c.selectionPos = c.stack->back()->transform.pos();
    m_cardSelected = c;
}

auto ScoundrelBoardManager::deselectCard() -> void
{
    m_cardSelected = std::nullopt;
}

auto ScoundrelBoardManager::getCardSelected() ->
        std::optional<CardClickedScoundrel>
{
    return m_cardSelected;
}

auto ScoundrelBoardManager::board() -> ScoundrelBoard&
{
    return m_board;
}

auto ScoundrelBoardManager::boardMap() -> ScoundrelBoardMap&
{
    return m_boardMap;
}


