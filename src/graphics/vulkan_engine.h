#pragma once

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <vector>
#include <array>
#include <GLFW/glfw3.h>

#include "model.h"

constexpr uint32_t maxFramesInFlight{ 2 };

struct ShaderData
{
    glm::mat4 projection;
    glm::mat4 view;
    glm::mat4 model[3];
    glm::vec4 lightPos{ 0.0f, -10.0f, 10.0f, 0.0f };
    uint32_t selected{ 1 };
};

struct ShaderDataBuffer
{
    VmaAllocation allocation{ VK_NULL_HANDLE };
    VmaAllocationInfo allocationInfo{};
    VkBuffer buffer{ VK_NULL_HANDLE };
    VkDeviceAddress deviceAddress{};
};

struct Texture
{
	VmaAllocation allocation{ VK_NULL_HANDLE };
	VkImage image{ VK_NULL_HANDLE };
	VkImageView view{ VK_NULL_HANDLE };
	VkSampler sampler{ VK_NULL_HANDLE };
};

class VulkanEngine
{
public:
    auto init(GLFWwindow* window) -> void;
    auto render() -> void;
    auto terminate() -> void;

    auto loadMeshData(std::vector<Vertex>& vertices, std::vector<uint16_t>& indices) -> void;
    auto loadShader(size_t bufferSize, uint32_t* bufferPointer) -> void;
    auto setUniformData(glm::mat4 proj, glm::mat4 model) -> void;
    auto createPipeline() -> void;

private:
    VkInstance m_instance{ VK_NULL_HANDLE };
    VkPhysicalDevice m_physicalDevice{ VK_NULL_HANDLE };
    VkDevice m_device{ VK_NULL_HANDLE };
    VkQueue m_queue{ VK_NULL_HANDLE };
    VmaAllocator m_allocator{ VK_NULL_HANDLE };
    VkSurfaceKHR m_surface{ VK_NULL_HANDLE };

    VkSwapchainKHR m_swapchain{ VK_NULL_HANDLE };
    std::vector<VkImage> m_swapchainImages;
    std::vector<VkImageView> m_swapchainImageViews;
	uint32_t m_imageCount;

    VkImage m_depthImage;
    VmaAllocation m_depthImageAllocation;
    VkImageView m_depthImageView;

    VkBuffer m_vBuffer{ VK_NULL_HANDLE };
    VmaAllocation m_vBufferAllocation{ VK_NULL_HANDLE };
	VkDeviceSize m_vBufSize;
	VkDeviceSize m_indexCount;

    VkShaderModule m_shaderModule;

    ShaderData m_shaderData{};
    std::array<ShaderDataBuffer, maxFramesInFlight> m_shaderDataBuffers;

    std::array<VkFence, maxFramesInFlight> m_fences;
    std::array<VkSemaphore, maxFramesInFlight> m_presentSemaphores;
    std::vector<VkSemaphore> m_renderSemaphores;

    VkCommandPool m_commandPool{ VK_NULL_HANDLE };
    std::array<VkCommandBuffer, maxFramesInFlight> m_commandBuffers;

    std::array<Texture, 3> m_textures{};

    VkDescriptorPool m_descriptorPool{ VK_NULL_HANDLE };
    VkDescriptorSetLayout m_descriptorSetLayoutTex{ VK_NULL_HANDLE };
    VkDescriptorSet m_descriptorSetTex{ VK_NULL_HANDLE };

    VkPipeline m_pipeline{ VK_NULL_HANDLE };
    VkPipelineLayout m_pipelineLayout{ VK_NULL_HANDLE };

    uint32_t m_frameIndex{ 0 };
    uint32_t m_imageIndex{ 0 };

    glm::ivec2 m_windowSize{};
    GLFWwindow* m_window;
};
