#include "dealer.h"

#include <cmath>

Dealer::Dealer() : m_randomEngine(m_r())
{
    createDeck();
}

void Dealer::createDeck()
{
    constexpr const int deckSize = 52;
    constexpr const int suitSize = 4;
    constexpr const int cardSize = 13;
    constexpr const int texCardsPerRow = 8;
    constexpr const float texTile = 0.125f;
    constexpr const float backTileX = 0.0f;
    constexpr const float backTileY = 0.875f;

    m_deck.reserve(deckSize);
    int count = 0;
    for (int i = 0; i < suitSize; i++)
    {
        for (int j = 0; j < cardSize; j++)
        {
            glm::vec2 uvOffset = {
                static_cast<float>(count % texCardsPerRow) * texTile,
                std::floorf(static_cast<float>(count) * texTile) * texTile,
            };
            m_deck.emplace_back(j, i, uvOffset, glm::vec2{ backTileX, backTileY });
            count++;
        }
    }
}

auto Dealer::shuffleDeck() -> void
{
    std::uniform_int_distribution<int> unifomDist(0,
            static_cast<int>(m_deck.size()) - 1);
    for (auto& card : m_deck)
    {
        int index = unifomDist(m_randomEngine);
        swapCard(card, m_deck[index]);
    }
}

auto Dealer::turnCardsDown() -> void
{
    for (auto& c : m_deck)
    {
        c.uvOffset = c.backfaceUv;
    }
}

auto Dealer::turnCardsUp() -> void
{
    for (auto& c : m_deck)
    {
        c.uvOffset = c.frontfaceUv;
    }
}

auto Dealer::fillTableau(std::span<CardStack> tableau, const std::span<float> tableauXMap, const std::span<float> tableauYMap) -> void
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
            m_deck[index].pos = glm::vec3(tableauXMap[i], tableauYMap[j], 0.0);
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
            m_deck[index].pos = glm::vec3(tableauXMap[i], tableauYMap[j], 0.0);
            tableau[i].emplace_back(&m_deck[index]);
            index++;
        }
    }
}

auto Dealer::emptyTable(std::span<CardStack> tableau, std::span<CardStack> openCells, std::span<CardStack> foundations) -> void
{
    for (auto& stack : tableau)
        stack = CardStack();

    for (auto& stack : openCells)
        stack = CardStack();

    for (auto& stack : foundations)
        stack = CardStack();
}

auto Dealer::swapCard(Card& a, Card& b) -> void
{
    auto tmp = a;
    a = b;
    b = tmp;
}

