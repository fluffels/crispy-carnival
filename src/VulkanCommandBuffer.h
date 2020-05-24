#pragma once

#include <vulkan/vulkan.h>

VkCommandPool createCommandPool(VkDevice, uint32_t, bool=false);
VkCommandBuffer allocateCommandBuffer(VkDevice, VkCommandPool);
void beginOneOffCommandBuffer(VkCommandBuffer buffer);
void beginFrameCommandBuffer(VkCommandBuffer buffer);
void endCommandBuffer(VkCommandBuffer);
