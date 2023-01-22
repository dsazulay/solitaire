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
        glm::vec2 mousePos = glm::vec2(Window::xPos, Window::yPos);
        for (Card* c : cards)
        {
            c->dragOffset = mousePos - glm::vec2(c->pos);
        }
        isDone = false;
    }

    void update()
    {
        glm::vec2 mousePos = glm::vec2(Window::xPos, Window::yPos);
        for (Card* c : m_cards)
        {
            c->pos = glm::vec3(mousePos - c->dragOffset, 0.0001);
        }
    }

    void stop()
    {
        isDone = true;
    }

    bool isDone{true};

private:
    std::span<Card*> m_cards;
};