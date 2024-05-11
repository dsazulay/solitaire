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

auto Dealer::deck() -> std::vector<CardEntity>&
{
    return m_deck;
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

