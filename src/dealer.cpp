#include "dealer.h"

#include <cmath>

Dealer::Dealer() : m_randomEngine(m_r()) {}

auto Dealer::createFreecellDeck() -> void
{
    constexpr const int deckSize = 52;
    constexpr const int suitSize = 4;
    constexpr const int cardSize = 13;
    constexpr const int texCardsPerRow = 8;
    constexpr const float texTile = 0.125f;

    m_deck.reserve(deckSize);
    m_deckUVs.reserve(deckSize);
    int count = 0;
    for (int i = 0; i < suitSize; i++)
    {
        for (int j = 0; j < cardSize; j++)
        {
            glm::vec2 uvOffset = {
                static_cast<float>(count % texCardsPerRow) * texTile,
                std::floorf(static_cast<float>(count) * texTile) * texTile,
            };
            m_deck.emplace_back(j, i, uvOffset);
            m_deckUVs.emplace_back(uvOffset);
            count++;
        }
    }
}

auto Dealer::createScoundrelDeck() -> void
{
    constexpr const int deckSize = 44;
    constexpr const int suitSize = 4;
    constexpr const int cardSize = 13;
    constexpr const int texCardsPerRow = 8;
    constexpr const float texTile = 0.125f;

    m_deck.reserve(deckSize);
    m_deckUVs.reserve(deckSize);
    int count = 0;
    for (int i = 0; i < suitSize; i++)
    {
        for (int j = 0; j < cardSize; j++)
        {
            glm::vec2 uvOffset = {
                static_cast<float>(count % texCardsPerRow) * texTile,
                std::floorf(static_cast<float>(count) * texTile) * texTile,
            };

            count++;
            if ((i == 0 || i == 2) && (j == 0 || j > 9)) { continue; }
            m_deck.emplace_back(j, i, uvOffset);
            m_deckUVs.emplace_back(uvOffset);
        }
    }
}

auto Dealer::shuffleDeck() -> void
{
    std::uniform_int_distribution<int> unifomDist(0,
            static_cast<int>(m_deck.size()) - 1);

    auto uv = m_deckUVs.begin();
    for (auto& card : m_deck)
    {
        int index = unifomDist(m_randomEngine);
        swapCard(card, m_deck[index]);
        swapUVs(*uv, m_deckUVs[index]);
        ++uv;
    }
}

auto Dealer::turnCardsDown() -> void
{
    constexpr const glm::vec2 backTile{ 0.0, 0.875 };
    for (auto& c : m_deck)
    {
        c.sprite.uv = backTile;
    }
}

auto Dealer::turnCardsUp() -> void
{
    for (int i = 0; i < m_deck.size(); ++i)
    {
        m_deck[i].sprite.uv = m_deckUVs[i];
    }
}

auto Dealer::fillTableau(std::span<CardStack> tableau,
        const std::span<float> tableauXMap,
        const std::span<float> tableauYMap) -> void
{
    constexpr const int reserveSize = 10;
    constexpr const int fullStackSize = 7;

    int tableauSize = static_cast<int>(tableau.size());
    int tableauHalfSize = tableauSize / 2;
    int index = 0;
    for (int i = 0; i < tableauHalfSize; i++)
    {
        tableau[i].reserve(reserveSize);
        for (int j = 0; j < fullStackSize; j++)
        {
            m_deck[index].transform.pos(glm::vec3(tableauXMap[i],
                        tableauYMap[j], 0.0));
            tableau[i].emplace_back(&m_deck[index]);
            index++;
        }
    }

    constexpr const int stackSize = 6;
    for (int i = tableauHalfSize; i < tableauSize; i++)
    {
        tableau[i].reserve(reserveSize);
        for (int j = 0; j < stackSize; j++)
        {
            m_deck[index].transform.pos(glm::vec3(tableauXMap[i],
                        tableauYMap[j], 0.0));
            tableau[i].emplace_back(&m_deck[index]);
            index++;
        }
    }
}

auto Dealer::emptyTable(std::span<CardStack> tableau,
        std::span<CardStack> openCells, std::span<CardStack> foundations) -> void
{
    for (auto& stack : tableau)
        stack = CardStack();

    for (auto& stack : openCells)
        stack = CardStack();

    for (auto& stack : foundations)
        stack = CardStack();
}

auto Dealer::fillDungeon(CardStack& dungeon, glm::vec2 pos) -> void
{
    dungeon.reserve(m_deck.size());
    for (auto& card : m_deck)
    {
        card.transform.pos(glm::vec3{ pos, 0.0 });
        dungeon.emplace_back(&card);
    }
}

auto Dealer::swapCard(CardEntity& a, CardEntity& b) -> void
{
    auto tmp = a;
    a = b;
    b = tmp;
}

auto Dealer::swapUVs(glm::vec2& a, glm::vec2& b) -> void
{
    auto tmp = a;
    a = b;
    b = tmp;
}

