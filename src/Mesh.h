#pragma once

#include "Vulkan.h"

struct Mesh {
    VulkanBuffer vBuff;
    uint32_t idxCount;
};

void uploadVertexDataFromObj(Vulkan& vk, char* filename, Mesh& mesh);
