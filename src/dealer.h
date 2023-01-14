#pragma once

#include <vector>
#include <array>

#include "card.h"

class Dealer
{
public:
    Dealer();
    void createDeck();
    void shuffleDeck();
    void fillTableau(std::array<CardStack, 8>& tableau, const std::array<std::array<glm::vec2, 12>, 8>& tableauMap);
    static void emptyTable(std::array<CardStack, 8>& tableau, std::array<CardStack, 4>& openCells, std::array<CardStack, 4>& foundations);
private:
    void swapCard(Card& a, Card& b);

    std::vector<Card> m_deck;
};
