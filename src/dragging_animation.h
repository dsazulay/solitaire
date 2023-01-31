#pragma once

#include <span>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "window.h"
#include "card.h"

class DraggingAnimation
{
public:
    void start(std::span<Card*> cards)
    {
        m_cards = cards;
        for (Card* c : cards)
        {
            c->dragOffset = Window::mousePos - glm::vec2(c->pos);
        }
        m_isDone = false;
    }

    void update()
    {
        for (Card* c : m_cards)
        {
            c->pos = glm::vec3(Window::mousePos - c->dragOffset, zOffset);
        }
    }

    void stop()
    {
        m_isDone = true;
    }

    auto isDone() -> bool
    {
        return m_isDone;
    }

private:
    constexpr static float zOffset = 0.0001f;

    std::span<Card*> m_cards;
    bool m_isDone{true};
};