#include "dealer.h"

#include <cstdlib>

Dealer::Dealer()
{
    createDeck();
}

void Dealer::createDeck()
{
    m_deck.reserve(52);
    int count = 0;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 13; j++)
        {
            m_deck.emplace_back(j, i, (count % 8) * 0.125f, static_cast<int>(count * 0.125f) * 0.125f);
            count++;
        }
    }
}

void Dealer::shuffleDeck()
{
    for (int i = m_deck.size() - 1; i >= 0; i--)
    {
        int j = rand() % (i + 1);
        swapCard(m_deck[i], m_deck[j]);
    }
}

void Dealer::fillTableau(std::array<CardStack, 8>& tableau, const std::array<std::array<glm::vec2, 12>, 8>& tableauMap)
{
    int index = 0;
    for (int i = 0; i < 4; i++)
    {
        tableau[i].reserve(10);
        for (int j = 0; j < 7; j++)
        {
            m_deck[index].pos = glm::vec3(tableauMap[i][j], 0.0);
            tableau[i].emplace_back(&m_deck[index]);
            index++;
        }
    }

    for (int i = 4; i < 8; i++)
    {
        tableau[i].reserve(10);
        for (int j = 0; j < 6; j++)
        {
            m_deck[index].pos = glm::vec3(tableauMap[i][j], 0.0);
            tableau[i].emplace_back(&m_deck[index]);
            index++;
        }
    }
}

void Dealer::emptyTable(std::array<CardStack, 8>& tableau, std::array<CardStack, 4>& openCells, std::array<CardStack, 4>& foundations)
{
    for (int i = 0; i < 8; i++)
    {
        tableau[i] = CardStack();
    }

    for (int i = 0; i < 4; i++)
    {
        openCells[i] = CardStack();
        foundations[i] = CardStack();
    }
}

void Dealer::swapCard(Card& a, Card& b)
{
    auto tmp = a;
    a = b;
    b = tmp;
}
