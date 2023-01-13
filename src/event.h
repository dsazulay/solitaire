#pragma once

#include "keycodes.h"

class Event
{
public:
     Event() = default;
     virtual const char* type() const = 0;
};

class MouseClickEvent : public Event
{
public:
    MouseClickEvent(double xPos, double yPos) : m_xPos(xPos), m_yPos(yPos) {}
    const char* type() const override { return descriptor; }
    double xPos() const { return m_xPos; }
    double yPos() const { return m_yPos; }

    static constexpr char* descriptor = (char*) "MouseClickEvent";
private:
    double m_xPos;
    double m_yPos;
};

class MouseDoubleClickEvent : public Event
{
public:
    MouseDoubleClickEvent(double xPos, double yPos) : m_xPos(xPos), m_yPos(yPos) {}
    const char* type() const override { return descriptor; }
    double xPos() const { return m_xPos; }
    double yPos() const { return m_yPos; }

    static constexpr char* descriptor = (char*) "MouseDoubleClickEvent";
private:
    double m_xPos;
    double m_yPos;
};

class MouseDragStartEvent : public Event
{
public:
    MouseDragStartEvent(double xPos, double yPos) : m_xPos(xPos), m_yPos(yPos) {}
    const char* type() const override { return descriptor; }
    double xPos() const { return m_xPos; }
    double yPos() const { return m_yPos; }

    static constexpr char* descriptor = (char*) "MouseDragStartEvent";
private:
    double m_xPos;
    double m_yPos;
};

class MouseDragEndEvent : public Event
{
public:
    MouseDragEndEvent(double xPos, double yPos) : m_xPos(xPos), m_yPos(yPos) {}
    const char* type() const override { return descriptor; }
    double xPos() const { return m_xPos; }
    double yPos() const { return m_yPos; }

    static constexpr char* descriptor = (char*) "MouseDragEndEvent";
private:
    double m_xPos;
    double m_yPos;
};

class KeyboardPressEvent : public Event
{
public:
    KeyboardPressEvent(KeyCode key) : m_key(key) {}
    const char* type() const override { return descriptor; }
    KeyCode key() const { return m_key; }

    static constexpr char* descriptor = (char*) "KeyboardPressEvent";
private:
    KeyCode m_key;
};

class GameWinEvent : public Event
{
public:
    GameWinEvent() {}
    const char* type() const override { return descriptor; }

    static constexpr char* descriptor = (char*) "GameWinEvent";
};

class UiNewGameEvent : public Event
{
public:
    UiNewGameEvent() {}
    const char* type() const override { return descriptor; }

    static constexpr char* descriptor = (char*) "UiNewGameEvent";
};
