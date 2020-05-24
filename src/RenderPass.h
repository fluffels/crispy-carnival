#pragma once

#include <vector>

#include "Vulkan.h"

using std::vector;

struct RenderPass {
    vector<VkCommandBuffer> cmds;
    VulkanBuffer vBuff;
    VulkanBuffer iBuff;
    uint32_t idxCount;
    vector<VulkanSampler> samplers;
};

void initRenderPass(Vulkan&, RenderPass&);
