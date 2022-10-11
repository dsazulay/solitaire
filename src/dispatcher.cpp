#include "dispatcher.h"

void Dispatcher::subscribe(const char* descriptor, std::function<void(const Event &)>&& callback)
{
    m_observers[descriptor].push_back(callback);
}

void Dispatcher::post(const Event &event)
{
    auto type = event.type();

    if (m_observers.find(type) == m_observers.end())
        return;

    auto&& observers = m_observers.at(type);

    for (auto&& observer : observers)
    {
        observer(event);
    }
}

Dispatcher& Dispatcher::instance()
{
    static Dispatcher instance_;
    return instance_;
}
