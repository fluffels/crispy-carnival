#pragma once

#include <vector>

#include "Vulkan.h"

using std::vector;

struct Mesh {
    VulkanBuffer vBuff;
    VulkanBuffer iBuff;
    uint32_t idxCount;
};

struct Brush {
    vector<VkCommandBuffer> cmds;
    Mesh mesh;
    Mesh mesh2;
    VulkanSampler skybox;
};

void initBrush(Vulkan&, Brush&);
