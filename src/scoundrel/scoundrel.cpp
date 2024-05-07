#include "scoundrel.h"

constexpr const int MAX_LIFE = 20;

auto Scoundrel::init() -> void
{
    createBgCards();
    m_dealer.createScoundrelDeck();
    m_dealer.shuffleDeck();
    m_dealer.fillDungeon(m_board.dungeon, glm::vec2{ 0.0 });
    fillRoom();
    m_board.updateCardList();
    m_life = MAX_LIFE;
}

auto Scoundrel::createBgCards() -> void
{
    constexpr const glm::vec2 cardbackUV = { 0.0, 0.875f };
    constexpr const glm::vec2 weaponUV = { 0.625, 0.875f };
    constexpr const glm::vec2 potionUV = { 0.500, 0.875f };
    constexpr const glm::vec2 handsUV = { 0.375, 0.875f };

    m_cardBg[0] = CardBg(cardbackUV, glm::vec3{ m_boardMap.dungeon, 0.0 });
    m_cardBg[1] = CardBg(weaponUV, glm::vec3{ m_boardMap.weapon[0], 0.0 });
    m_cardBg[2] = CardBg(potionUV, glm::vec3{ m_boardMap.potion, 0.0 });
    m_cardBg[3] = CardBg(handsUV, glm::vec3{ m_boardMap.hands, 0.0 });

    m_board.cardBgs = m_cardBg;
}

auto Scoundrel::fillRoom() -> void
{
    int startIndex = 2;
    // move remaining card to last position
    for (auto c : m_board.room)
    {
        if (c != nullptr)
        {
            m_board.room[3] = c;
            m_board.room[3]->transform.pos(glm::vec3{ m_boardMap.room[3], 0.0 });
            break;
        }

        // if we get here, it means there's no card in the room
        startIndex = 3;
    }

    for (int i = startIndex; i >= 0; --i)
    {
        m_board.room[i] = m_board.dungeon.back();
        m_board.dungeon.pop_back();
        m_board.room[i]->transform.pos(glm::vec3{ m_boardMap.room[i], 0.0 });
    }
}

auto Scoundrel::board() -> ScoundrelBoard&
{
    return m_board;
}

