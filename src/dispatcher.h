#pragma once

#include "event.h"
#include <functional>
#include <unordered_map>
#include <string>
#include <vector>


class Dispatcher
{
public:
    Dispatcher(const Dispatcher& other) = delete;
    void operator=(const Dispatcher& other) = delete;

    void subscribe(const char* descriptor, std::function<void(const Event&)>&& callback);
    void post(const Event& event);

    static Dispatcher& instance();
private:
    Dispatcher() = default;

    std::unordered_map<std::string, std::vector<std::function<void(const Event&)>>> m_observers;
};
