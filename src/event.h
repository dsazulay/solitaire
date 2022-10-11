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
    MouseClickEvent(double xpos, double ypos) : m_xpos(xpos), m_ypos(ypos) {}
    const char* type() const override { return descriptor; }
    double xpos() const { return m_xpos; }
    double ypos() const { return m_ypos; }

    static constexpr char* descriptor = (char*) "MouseClickEvent";
private:
    double m_xpos;
    double m_ypos;
};
