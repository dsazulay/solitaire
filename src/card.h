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

    Card(int number_, int suit_, float uvOffsetX, float uvOffsetY) : number(number_), 
        suit(suit_), uvOffset(uvOffsetX, uvOffsetY)
    {
    }

    Card(const Card& c) : number(c.number), suit(c.suit), uvOffset(c.uvOffset), 
        dragOffset(c.dragOffset), pos(c.pos)
    {
    }

    Card& operator=(const Card& c) = default;
};

typedef std::vector<Card*> CardStack;
