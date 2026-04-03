#pragma once

#include "../card.h"
#include <span>

class IGameHandler
{
public:
    virtual ~IGameHandler() = default;
    virtual auto update() -> void = 0;
    virtual auto cards() -> std::span<CardEntity*> = 0;
    virtual auto cardBgs() -> std::span<CardBg> = 0;
};
