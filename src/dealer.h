#pragma once

#include <vector>
#include <span>
#include <random>
#include <glm/vec2.hpp>

#include "card.h"

class Dealer
{
public:
    Dealer();
    auto createFreecellDeck() -> void;
    auto createScoundrelDeck() -> void;
    auto shuffleDeck() -> void;
    auto turnCardsDown() -> void;
    auto turnCardsUp() -> void;

    // freecell
    auto fillTableau(std::span<CardStack> tableau, std::span<float> tableauXMap,
            std::span<float> tableauYMap) -> void;
    auto emptyTable(std::span<CardStack> tableau, std::span<CardStack> openCells,
            std::span<CardStack> foundations) -> void;

    // scoundrel
    auto fillDungeon(CardStack& dungeon, glm::vec2 pos) -> void;

private:
    auto swapCard(Card& a, Card& b) -> void;

    std::vector<Card> m_deck;
    std::random_device m_r;
    std::default_random_engine m_randomEngine;
};
