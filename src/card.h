#pragma once

#include <vector>
#include <string_view>
#include "components.h"
#include "utils/log.h"

struct CardEntity
{
    Transform transform;
    Sprite sprite;
    Card card;

    CardEntity() = default;
    CardEntity(int number, int suit, glm::vec2 uvOffset,
            glm::vec3 pos = glm::vec3{ 0.0 })
    {
        card.number = number;
        card.suit = suit;
        sprite.uv = uvOffset;
        transform.pos(pos);
    }

    auto print() -> void
    {
        std::string_view suit_name;
        switch (card.suit)
        {
        case 0:
            suit_name = "hearts";
            break;
        case 1:
            suit_name = "spades";
            break;
        case 2:
            suit_name = "diamonds";
            break;
        case 3:
            suit_name = "clubs";
            break;
        }
        const glm::vec3& pos = transform.pos();
        LOG_INFO("{} of {} - (x: {}, y: {}, z: {})", card.number + 1,
                suit_name, pos.x, pos.y, pos.z);
    }
};

struct CardBg
{
    Transform transform;
    Sprite sprite;
};

using CardStack = std::vector<CardEntity*>;

