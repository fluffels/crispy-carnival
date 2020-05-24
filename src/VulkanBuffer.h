#pragma once

#include <vulkan/vulkan.h>

struct VulkanBuffer {
    VkBuffer handle;
    VkDeviceMemory memory;
    VkBufferView view;
};

void createVulkanBuffer(
    VkDevice,
    uint32_t,
    VkBufferUsageFlags,
    uint32_t,
    VulkanBuffer&
);

void allocateVulkanBuffer(
    VkDevice,
    VkPhysicalDeviceMemoryProperties,
    VulkanBuffer&
);

void createUniformBuffer(
    VkDevice device,
    VkPhysicalDeviceMemoryProperties& memories,
    uint32_t queueFamily,
    uint32_t size,
    VulkanBuffer& buffer
);

void createVertexBuffer(
    VkDevice device,
    VkPhysicalDeviceMemoryProperties& memories,
    uint32_t queueFamily,
    uint32_t size,
    VulkanBuffer& buffer
);

void createIndexBuffer(
    VkDevice device,
    VkPhysicalDeviceMemoryProperties& memories,
    uint32_t queueFamily,
    uint32_t size,
    VulkanBuffer& buffer
);
