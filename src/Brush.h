#pragma once

#include <vector>

#include "Vulkan.h"
#include "VulkanPipeline.h"

using std::vector;

struct Mesh {
    VulkanBuffer vBuff;
    uint32_t idxCount;
};

struct Brush {
    Mesh mesh;
    VulkanPipeline pipeline;
};

void initBrush(Vulkan&, Brush&);
void uploadVertexDataFromObj(Vulkan&, Brush&, char*);
