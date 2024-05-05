#include "scoundrel.h"

constexpr const int MAX_LIFE = 20;

auto Scoundrel::init() -> void
{
    m_boardMap.setBoardLayout();
    createBgCards();
    m_dealer.createScoundrelDeck();
    m_dealer.shuffleDeck();
    m_dealer.fillDungeon(m_board.dungeon, glm::vec2{ 0.0 });
    m_board.updateCards();
    m_life = MAX_LIFE;
}

auto Scoundrel::createBgCards() -> void
{
    constexpr const glm::vec2 cardbackUV = { 0.0, 0.875f };
    constexpr const glm::vec2 weaponUV = { 0.125, 0.875f };
    constexpr const glm::vec2 potionUV = { 0.250, 0.875f };
    constexpr const glm::vec2 handsUV = { 0.375, 0.875f };

    m_cardBg[0] = Card(-1, -1, cardbackUV, glm::vec2{ 0.0 },
            glm::vec3{ m_boardMap.dungeon, 0.0 });
    m_cardBg[1] = Card(-1, -1, weaponUV, glm::vec2{ 0.0 },
            glm::vec3{ m_boardMap.weapon[0], 0.0 });
    m_cardBg[2] = Card(-1, -1, potionUV, glm::vec2{ 0.0 },
            glm::vec3{ m_boardMap.potion, 0.0 });
    m_cardBg[3] = Card(-1, -1, weaponUV, glm::vec2{ 0.0 },
            glm::vec3{ m_boardMap.hands, 0.0 });

    m_board.cardsBg[0] = &m_cardBg[0];
    m_board.cardsBg[1] = &m_cardBg[1];
    m_board.cardsBg[2] = &m_cardBg[2];
    m_board.cardsBg[3] = &m_cardBg[3];
}

auto Scoundrel::board() -> ScoundrelBoard&
{
    return m_board;
}

