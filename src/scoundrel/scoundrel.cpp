#include "scoundrel.h"

constexpr const int MAX_LIFE = 20;

auto Scoundrel::init() -> void
{
    createBgCards();
    m_boardManager.createDeck();
    m_boardManager.shuffleDeck();
    m_boardManager.fillTable();
    m_boardManager.fillRoom();
    m_boardManager.updateCardList();
    m_life = MAX_LIFE;
}

auto Scoundrel::createBgCards() -> void
{
    auto& m_boardMap = m_boardManager.boardMap();
    constexpr const glm::vec2 cardbackUV = { 0.0, 0.875f };
    constexpr const glm::vec2 weaponUV = { 0.625, 0.875f };
    constexpr const glm::vec2 potionUV = { 0.500, 0.875f };
    constexpr const glm::vec2 handsUV = { 0.375, 0.875f };

    m_cardBg[0] = CardBg(cardbackUV, glm::vec3{ m_boardMap.dungeon, 0.0 });
    m_cardBg[1] = CardBg(weaponUV, glm::vec3{ m_boardMap.weapon[0], 0.0 });
    m_cardBg[2] = CardBg(potionUV, glm::vec3{ m_boardMap.potion, 0.0 });
    m_cardBg[3] = CardBg(handsUV, glm::vec3{ m_boardMap.hands, 0.0 });

    m_boardManager.board().cardBgs = m_cardBg;
}

auto Scoundrel::board() -> ScoundrelBoard&
{
    return m_boardManager.board();
}

auto Scoundrel::handleClick(double xpos, double ypos, bool isDragging,
            bool isDragStart) -> void
{
    auto cardClicked = m_boardManager.getStackAndPos(xpos, ypos);
}

auto Scoundrel::handleDoubleClick(double xpos, double ypos) -> void
{
    LOG_ERROR("Scoundrel double click");
}

auto Scoundrel::handleNewGame() -> void
{
    LOG_ERROR("scoundrel new game");
}

