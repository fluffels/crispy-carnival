#pragma once

#include "VulkanPipeline.h"

void
recordTextCommandBuffers(Vulkan& vk, vector<VkCommandBuffer>& cmds, char* text);
void
resetTextCommandBuffers(Vulkan& vk, vector<VkCommandBuffer>& cmds);