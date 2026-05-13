#pragma once

#include "card.h"
#include "utils/types.h"

#include <functional>
#include <glm/vec2.hpp>
#include <vector>

class Dealer
{
public:
    auto createFreecellDeck() -> void;
    auto createScoundrelDeck() -> void;
    auto shuffleDeck() -> void;
    auto turnCardsDown() -> void;
    auto turnCardsUp() -> void;
    auto deck() -> std::vector<CardEntity>&;

private:
    auto createDeck(i32 deckSize, std::function<bool(i32, i32)> filter = nullptr) -> void;
    template<typename T>
    auto swap(T& a, T& b) -> void;

    std::vector<CardEntity> m_deck;
    std::vector<glm::vec2> m_deckUVs;
};

template<typename T>
auto Dealer::swap(T& a, T& b) -> void
{
    auto tmp = a;
    a = b;
    b = tmp;
}
