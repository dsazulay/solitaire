#pragma once

#include <cstddef>
#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <vector>
#include <array>
#include <GLFW/glfw3.h>

#include "model.h"
#include "../utils/handle.h"

constexpr uint32_t MAX_FRAMES_IN_FLIGHT{ 2 };

struct ShaderTag {};
struct PipelineTag {};

using ShaderID = Handle<ShaderTag>;
using PipelineID = Handle<PipelineTag>;

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

struct MeshBuffer
{
    VkBuffer buffer{ VK_NULL_HANDLE };
    VmaAllocation allocation{ VK_NULL_HANDLE };
    VkDeviceSize bufferSize{};
    VkDeviceSize indexCount{};
};

struct GameObject
{
    size_t meshID;
    size_t pipelineID;
    ShaderData shaderData;
    std::array<ShaderDataBuffer, MAX_FRAMES_IN_FLIGHT> shaderDataBuffers;
};

class VulkanEngine
{
public:
    auto init(GLFWwindow* window) -> void;
    auto render() -> void;
    auto terminate() -> void;

    auto loadMeshData(std::vector<Vertex>& vertices, std::vector<uint16_t>& indices) -> size_t;
    auto loadShader(size_t bufferSize, uint32_t* bufferPointer) -> ShaderID;
    auto setUniformData(size_t id, glm::mat4 proj, glm::mat4 model) -> void;
    auto createPipeline(ShaderID shaderID) -> PipelineID;
    auto createUniformBuffers() -> void;
    auto addGameObject(size_t id, PipelineID pipelineID) -> size_t;

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

    std::vector<MeshBuffer> m_meshBuffers;
    std::vector<GameObject> m_gameObjects;

    std::vector<VkShaderModule> m_shaderModules;
    std::vector<VkPipeline> m_pipelines;

    std::array<VkFence, MAX_FRAMES_IN_FLIGHT> m_fences;
    std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> m_presentSemaphores;
    std::vector<VkSemaphore> m_renderSemaphores;

    VkCommandPool m_commandPool{ VK_NULL_HANDLE };
    std::array<VkCommandBuffer, MAX_FRAMES_IN_FLIGHT> m_commandBuffers;

    std::array<Texture, 1> m_textures{};

    VkDescriptorPool m_descriptorPool{ VK_NULL_HANDLE };
    VkDescriptorSetLayout m_descriptorSetLayoutTex{ VK_NULL_HANDLE };
    VkDescriptorSet m_descriptorSetTex{ VK_NULL_HANDLE };

    VkPipelineLayout m_pipelineLayout{ VK_NULL_HANDLE };

    uint32_t m_frameIndex{ 0 };
    uint32_t m_imageIndex{ 0 };

    glm::ivec2 m_windowSize{};
    GLFWwindow* m_window;
};
