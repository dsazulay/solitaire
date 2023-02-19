#pragma once

class Timer
{
public:
    static auto update() -> void;
    static auto halt() -> void;

    static float time;
    static float systemTime;
    static float deltaTime;
private:
    static float lastFrame;
};
