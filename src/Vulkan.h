#pragma once

#include <stdexcept>
#include <string>
#include <vector>

#include <vulkan/vulkan.h>

#include "VulkanBuffer.h"
#include "VulkanCommandBuffer.h"
#include "VulkanImage.h"
#include "VulkanMemory.h"

using std::string;
using std::runtime_error;
using std::vector;

struct VulkanPipeline {
    VkPipeline handle;
    VkPipelineLayout layout;
    VkRenderPass renderPass;
    VkShaderModule vertexShader;
    VkShaderModule fragmentShader;
    VkDescriptorSetLayout descriptorLayout;
    VkDescriptorPool descriptorPool;
    VkDescriptorSet descriptorSet;
};

struct VulkanSwapChain {
    VkPresentModeKHR presentMode;
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    VkSwapchainKHR handle;
    VkExtent2D extent;
    VkFormat format;
    VkColorSpaceKHR colorSpace;
    vector<VulkanImage> images;
    vector<VkFramebuffer> framebuffers;
    VkSurfaceKHR surface;
    VkSemaphore imageReady;
    VkSemaphore presentReady;
};

struct Vulkan {
    VkDebugReportCallbackEXT debugCallback;
    VkDevice device;
    VkInstance handle;
    VkPhysicalDevice gpu;
    vector<string> extensions;
    vector<string> layers;
    VkQueue queue;
    uint32_t queueFamily;
    VkPhysicalDeviceMemoryProperties memories;

    VulkanPipeline pipeline;
    VulkanSwapChain swap;

    VulkanImage depth;
    VulkanBuffer mvp;

    VkCommandPool cmdPool;
    VkCommandPool cmdPoolTransient;
};

void createFramebuffers(Vulkan&);
void createVKInstance(Vulkan& vk);
void initVK(Vulkan& vk);
void initVKPipeline(Vulkan& vk);
void initVKSwapChain(Vulkan& vk);
