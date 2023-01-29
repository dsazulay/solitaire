#pragma once

#include <span>
#include <utility>
#include <functional>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/gtx/compatibility.hpp>

#include "card.h"
#include "timer.h"

namespace moveanim
{
    const float ZOFFSET = 0.0001f;
    const float YOFFSET = 32.0f;
    const float ANIMSPEED = 4000.0f;
}

class MovingAnimation
{
public:
    MovingAnimation(std::span<Card *> cards, glm::vec2 startPos, glm::vec2 dstPos, std::function<void()> onComplete = nullptr)
        : m_startPos(startPos), m_dstPos(dstPos), m_cards(cards), m_onCompleteCallback(std::move(onComplete))
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

        float distCovered = (Timer::time - m_startTime) * m_speed;
        float delta = distCovered / m_len;
        glm::vec3 pos;
        if (delta >= 1.0f)
        {
            for (int i = 0; i < (int) m_cards.size(); i++)
            {
                pos = glm::vec3(m_dstPos, 0.0);
                pos.y -= (float) i * moveanim::YOFFSET;
                m_cards[i]->pos = pos;
                m_isDone = true;
            }
            if (m_onCompleteCallback != nullptr)
                m_onCompleteCallback();
        }
        else
        {
            for (int i = 0; i < (int) m_cards.size(); i++)
            {
                pos = glm::vec3(glm::lerp(m_startPos, m_dstPos, delta), moveanim::ZOFFSET);
                pos.y -= (float) i * moveanim::YOFFSET;
                m_cards[i]->pos = pos;
            }
        }        
    }

    auto isDone() -> bool
    {
        return m_isDone;
    }

private:
    float m_speed = moveanim::ANIMSPEED;
    float m_startTime{};
    float m_len;
    glm::vec2 m_startPos;
    glm::vec2 m_dstPos;
    std::span<Card*> m_cards;
    std::function<void()> m_onCompleteCallback;
    bool m_hasStarted{};
    bool m_isDone{};
};