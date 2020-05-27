#pragma once

#include <vector>

#include "Vulkan.h"

using std::vector;

struct Mesh {
    VulkanBuffer vBuff;
    VulkanBuffer iBuff;
    uint32_t idxCount;
};

struct RenderPass {
    vector<VkCommandBuffer> cmds;
    Mesh mesh;
    VulkanSampler skybox;
};

void initRenderPass(Vulkan&, RenderPass&);
