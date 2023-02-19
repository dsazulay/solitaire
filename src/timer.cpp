#include "timer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

float Timer::time = 0.0f;
float Timer::systemTime;
float Timer::deltaTime;
float Timer::lastFrame;

auto Timer::update() -> void
{
    systemTime = (float) glfwGetTime();
    deltaTime = systemTime - lastFrame;
    lastFrame = systemTime;
    time += deltaTime;
}

auto Timer::halt() -> void
{
    lastFrame = (float) glfwGetTime();
}
