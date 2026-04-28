#pragma once

#include "../utils/types.h"
#include "../card.h"
#include "../timer.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>

#include <span>
#include <utility>
#include <functional>

class MovingAnimation
{
public:
    MovingAnimation() = default;

    MovingAnimation(
        std::span<CardEntity *> cards,
        glm::vec2 startPos,
        glm::vec2 dstPos,
        std::function<void()> onComplete = nullptr
    ) :
        m_startPos{ startPos },
        m_dstPos{ dstPos },
        m_cards{ cards },
        m_onCompleteCallback{ std::move(onComplete) }
    {
        m_len = glm::max(glm::length(m_dstPos - m_startPos), 0.00001f);
    }

    void update()
    {
        // TODO: check if can find better way to initialize the start time
        if (!m_hasStarted)
        {
            m_startTime = Timer::time;
            m_hasStarted = true;
        }

        f32 distCovered = (Timer::time - m_startTime) * m_speed;
        f32 delta = distCovered / m_len;
        glm::vec3 pos;
        if (delta >= 1.0f)
        {
            for (u64 i = 0; i < m_cards.size(); ++i)
            {
                pos = glm::vec3(m_dstPos, 0.0);
                pos.y -= (f32) i * MovingAnimation::Y_OFFSET;
                m_cards[i]->transform.pos(pos);
                m_isDone = true;
            }
            if (m_onCompleteCallback != nullptr)
                m_onCompleteCallback();
        }
        else
        {
            for (u64 i = 0; i < m_cards.size(); ++i)
            {
                pos = glm::vec3(glm::lerp(m_startPos, m_dstPos, delta), MovingAnimation::Z_OFFSET);
                pos.y -= (f32) i * MovingAnimation::Y_OFFSET;
                m_cards[i]->transform.pos(pos);
            }
        }
    }

    auto isDone() -> bool
    {
        return m_isDone;
    }

private:
    constexpr static f32 Z_OFFSET = 0.0001f;
    constexpr static f32 Y_OFFSET = 30.0f;
    constexpr static f32 ANIMATION_SPEED = 4000.0f;

    f32 m_speed = MovingAnimation::ANIMATION_SPEED;
    f32 m_startTime{};
    f32 m_len;
    glm::vec2 m_startPos;
    glm::vec2 m_dstPos;
    std::span<CardEntity*> m_cards;
    std::function<void()> m_onCompleteCallback;
    bool m_hasStarted{};
    bool m_isDone{};
};
