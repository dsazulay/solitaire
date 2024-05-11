#pragma once

#include <vector>
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
    auto deck() -> std::vector<CardEntity>&;

private:
    auto swapCard(CardEntity& a, CardEntity& b) -> void;
    auto swapUVs(glm::vec2& a, glm::vec2& b) -> void;

    std::vector<CardEntity> m_deck;
    std::vector<glm::vec2> m_deckUVs;
    std::random_device m_r;
    std::default_random_engine m_randomEngine;
};
