#pragma once

#include <functional>
#include <vector>
#include <concepts>

#include "event.h"

template<class T, class U>
concept Derived = std::is_base_of<U, T>::value;

template<Derived<Event> EventType>
class Dispatcher
{
    using Callback = std::function<void(const EventType&)>;
public:
    Dispatcher(const Dispatcher& other) = delete;
    Dispatcher(Dispatcher&& other) = delete;
    auto operator=(const Dispatcher& other) -> void = delete;
    auto operator=(Dispatcher&& other) -> void = delete;
    ~Dispatcher() = default;

    auto subscribe(Callback&& callback) -> void
    {
        m_observers.push_back(callback);
    }

    auto post(const EventType& event) -> void
    {
        for (auto&& observer : m_observers)
        {
            observer(event);
        }
    }

    static auto instance() -> Dispatcher&
    {
        static Dispatcher instance_;
        return instance_;
    }

private:
    Dispatcher() = default;

    std::vector<Callback> m_observers;
};