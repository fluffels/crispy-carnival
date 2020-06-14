#pragma once

#include <vector>

#include "Mesh.h"
#include "Vulkan.h"
#include "VulkanPipeline.h"

using std::vector;

struct Brush {
    Mesh mesh;
    VulkanPipeline pipeline;
};

void initBrush(Vulkan&, Brush&);
