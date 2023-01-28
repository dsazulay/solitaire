#pragma once

#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

struct Card
{
    int number;
    int suit;
    glm::vec2 uvOffset;
    glm::vec2 dragOffset;
    glm::vec3 pos;

    Card() = default;

    Card(int number_, int suit_, float uvOffsetX, float uvOffsetY, glm::vec3 pos_ = glm::vec3(0.0)) : 
        number(number_), suit(suit_), uvOffset(uvOffsetX, uvOffsetY), dragOffset(0.0), pos(pos_)
    {
    }

    Card(const Card& c) = default;
    Card(Card&& c) = default;
    auto operator=(const Card& c) -> Card& = default;
    auto operator=(Card&& c) -> Card& = default;

    ~Card() = default;
};

using CardStack = std::vector<Card*>;
