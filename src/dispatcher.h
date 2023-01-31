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
    static auto subscribe(Callback&& callback) -> void
    {
        observers.push_back(callback);
    }

    static auto post(const EventType& event) -> void
    {
        for (auto&& observer : observers)
        {
            observer(event);
        }
    }

private:
    Dispatcher() = default;

    static std::vector<Callback> observers;
};

template<Derived<Event> EventType>
std::vector<std::function<void(const EventType&)>> Dispatcher<EventType>::observers;