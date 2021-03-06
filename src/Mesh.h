#pragma once

#include "Vulkan.h"

struct Mesh {
    VulkanBuffer vBuff;
    uint32_t idxCount;
    VulkanBuffer iBuff;
};

void uploadVertexDataFromObj(
    Vulkan& vk,
    VulkanPipeline& pipline,
    char* filename,
    Mesh& mesh
);
