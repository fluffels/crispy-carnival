#pragma once

#include <vulkan/vulkan.h>

struct VulkanImage {
    VkImage handle;
    VkImageView view;
    VkDeviceMemory memory;
};

struct VulkanSampler {
    VulkanImage image;
    VkSampler handle;
};

VulkanImage createVulkanDepthBuffer(
    VkDevice,
    VkPhysicalDeviceMemoryProperties&,
    VkExtent2D,
    uint32_t
);

VulkanSampler createVulkanSampler2D(
    VkDevice device,
    VkPhysicalDeviceMemoryProperties& memories,
    VkExtent2D extent,
    uint32_t family
);

VulkanSampler createVulkanSamplerCube(
    VkDevice device,
    VkPhysicalDeviceMemoryProperties& memories,
    VkExtent2D extent,
    uint32_t family
);

void destroyVulkanImage(
    VkDevice device,
    VulkanImage image
);

void destroyVulkanSampler(
    VkDevice device,
    VulkanSampler sampler
);

