#pragma once

#include "keycodes.h"

class Event
{
public:
    Event() = default;
    Event(const Event& e) = default;
    Event(Event&& e) = default;
    auto operator=(const Event& e) -> Event& = default;
    auto operator=(Event&& e) -> Event& = default;
    virtual ~Event() = default;
};

class MouseClickEvent : public Event
{
public:
    MouseClickEvent(double xPos, double yPos) : m_xPos(xPos), m_yPos(yPos) {}
    [[nodiscard]] auto xPos() const -> double { return m_xPos; }
    [[nodiscard]] auto yPos() const -> double { return m_yPos; }

private:
    double m_xPos;
    double m_yPos;
};

class MouseDoubleClickEvent : public Event
{
public:
    MouseDoubleClickEvent(double xPos, double yPos) : m_xPos(xPos), m_yPos(yPos) {}
    [[nodiscard]] auto xPos() const -> double { return m_xPos; }
    [[nodiscard]] auto yPos() const -> double { return m_yPos; }

private:
    double m_xPos;
    double m_yPos;
};

class MouseDragEvent : public Event
{
public:
    MouseDragEvent(double xPos, double yPos, bool isStart) : m_xPos(xPos), m_yPos(yPos), m_isStart(isStart) {}
    [[nodiscard]] auto xPos() const -> double { return m_xPos; }
    [[nodiscard]] auto yPos() const -> double { return m_yPos; }
    [[nodiscard]] auto isStart() const -> bool { return m_isStart; }

private:
    double m_xPos;
    double m_yPos;
    bool m_isStart;
};

class KeyboardPressEvent : public Event
{
public:
    KeyboardPressEvent(KeyCode key) : m_key(key) {}
    [[nodiscard]] auto key() const -> KeyCode { return m_key; }

private:
    KeyCode m_key;
};

class GameWinEvent : public Event {};

class UiNewGameEvent : public Event {};
