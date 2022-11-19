#pragma once

class Timer
{
public:
    static void update();

    static float time;
    static float deltaTime;
private:
    static float lastFrame;
};
