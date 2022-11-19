#pragma once

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
