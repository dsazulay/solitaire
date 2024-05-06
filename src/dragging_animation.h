#pragma once

#include <span>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "window.h"
#include "card.h"

constexpr const float Z_OFFSET = 0.0001f;
constexpr const int MAX_STACK_SIZE = 13;

class DraggingAnimation
{
public:
    void start(std::span<CardEntity*> cards)
    {
        m_cards = cards;
        for (int i = 0; i < m_cards.size(); ++i)
        {
            m_dragOffset[i] = Window::mousePos - glm::vec2(
                    m_cards[i]->transform.pos());
        }
        m_isDone = false;
    }

    void update()
    {
        for (int i = 0; i < m_cards.size(); ++i)
        {
            m_cards[i]->transform.pos(glm::vec3(
                        Window::mousePos - m_dragOffset[i], Z_OFFSET));
        }
    }

    void stop()
    {
        for (CardEntity* c : m_cards)
        {
            c->transform.posZ(0.0);
        }
        m_isDone = true;
    }

    auto isDone() -> bool
    {
        return m_isDone;
    }

private:

    std::span<CardEntity*> m_cards;
    std::array<glm::vec2, MAX_STACK_SIZE> m_dragOffset;
    bool m_isDone{true};
};
