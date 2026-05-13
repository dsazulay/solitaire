#include "dealer.h"

#include "utils/random.h"

#include <cmath>

auto Dealer::createFreecellDeck() -> void
{
    constexpr i32 deckSize = 52;
    createDeck(deckSize);
}

auto Dealer::createScoundrelDeck() -> void
{
    constexpr i32 deckSize = 44;
    createDeck(deckSize, [](i32 i, i32 j) {
        return (i == 0 || i == 2) && (j == 0 || j > 9);
    });
}

auto Dealer::shuffleDeck() -> void
{
    i32 maxIndex = (i32) (m_deck.size() - 1);
    auto uv = m_deckUVs.begin();
    for (auto& card : m_deck)
    {
        i32 index = Random::randInt(maxIndex);
        swap<CardEntity>(card, m_deck[index]);
        swap<glm::vec2>(*uv, m_deckUVs[index]);
        ++uv;
    }
}

auto Dealer::turnCardsDown() -> void
{
    constexpr glm::vec2 backTile{ 0.0f, 0.875f };
    for (auto& c : m_deck)
    {
        c.sprite.uv = backTile;
    }
}

auto Dealer::turnCardsUp() -> void
{
    for (u64 i = 0; i < m_deck.size(); ++i)
    {
        m_deck[i].sprite.uv = m_deckUVs[i];
    }
}

auto Dealer::deck() -> std::vector<CardEntity>&
{
    return m_deck;
}

auto Dealer::createDeck(i32 deckSize, std::function<bool(i32, i32)> filter) -> void
{
    constexpr i32 suitSize = 4;
    constexpr i32 cardSize = 13;
    constexpr i32 texCardsPerRow = 8;
    constexpr f32 texTile = 0.125f;

    m_deck.reserve(deckSize);
    m_deckUVs.reserve(deckSize);
    i32 count = 0;
    for (i32 i = 0; i < suitSize; ++i)
    {
        for (i32 j = 0; j < cardSize; ++j)
        {
            glm::vec2 uvOffset = {
                ((f32) (count % texCardsPerRow)) * texTile,
                std::floorf(((f32) count) * texTile) * texTile,
            };
            count++;
            if (filter != nullptr && filter(i, j)) { continue; }
            m_deck.emplace_back(j, i, uvOffset);
            m_deckUVs.emplace_back(uvOffset);
        }
    }
}

