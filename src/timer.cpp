#include "timer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

float Timer::time;
float Timer::deltaTime;
float Timer::lastFrame;

void Timer::update()
{
    time = (float) glfwGetTime();
    deltaTime = time - lastFrame;
    lastFrame = time;
}
