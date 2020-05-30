#pragma once

#include <vector>

#include "Mesh.h"
#include "Vulkan.h"

using std::vector;

struct Brush {
    vector<VkCommandBuffer> cmds;
    Mesh mesh;
    Mesh mesh2;
    VulkanSampler skybox;
};

void initBrush(Vulkan&, Brush&);
