#pragma once

#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>


struct Card
{
    int number;
    int suit;
    glm::vec2 uvOffset;
    glm::vec2 frontfaceUv;
    glm::vec2 backfaceUv;
    glm::vec2 dragOffset;
    glm::vec3 pos;

    Card() = default;
    Card(int number_, int suit_, glm::vec2 uvOffset_, glm::vec2 backfaceUv_,
            glm::vec3 pos_ = glm::vec3{ 0.0 }) :
        number(number_), suit(suit_), uvOffset(uvOffset_), frontfaceUv(uvOffset_),
        backfaceUv(backfaceUv_), dragOffset(0.0), pos(pos_)
    {
    }
};

using CardStack = std::vector<Card*>;

auto printCard(Card& card) -> void;

