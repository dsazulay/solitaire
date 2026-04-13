#pragma once

#include "vulkan_engine.h"

class VulkanRenderer
{
public:
    auto init(GLFWwindow* window) -> void;
    auto render() -> void;
    auto terminate() -> void;
private:
    VulkanEngine m_vulkanEngine;
};
