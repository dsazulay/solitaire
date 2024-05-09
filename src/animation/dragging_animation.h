#pragma once

#include <span>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "../window.h"
#include "../card.h"

constexpr const float Z_OFFSET = 0.0001f;
constexpr const int MAX_STACK_SIZE = 13;

class DraggingAnimation
{
public:
    DraggingAnimation() = default;
    DraggingAnimation(DraggingAnimation &other) = default;
    DraggingAnimation(DraggingAnimation &&other) = default;
    auto operator=(const DraggingAnimation &other) ->
        DraggingAnimation&  = default;
    auto operator=(DraggingAnimation &&other) ->
        DraggingAnimation& = default;
    ~DraggingAnimation() = default;

    DraggingAnimation(std::span<CardEntity*> cards) : m_cards{ cards }
    {
    }

    void start()
    {
        auto dragIter = m_dragOffset.begin();
        for (auto c = m_cards.begin(); c != m_cards.end(); ++c, ++dragIter)
        {
            *dragIter = Window::mousePos - glm::vec2((*c)->transform.pos());
        }
        m_isDone = false;
    }

    void update()
    {
        auto dragIter = m_dragOffset.begin();
        for (auto c = m_cards.begin(); c != m_cards.end(); ++c, ++dragIter)
        {
            (*c)->transform.pos(glm::vec3(Window::mousePos - *dragIter, Z_OFFSET));
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
    std::array<glm::vec2, MAX_STACK_SIZE> m_dragOffset{};
    bool m_isDone{true};
};
