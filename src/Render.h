#pragma once

#include <vector>

#include "Vulkan.h"

using std::vector;

void recordCommandBuffers(Vulkan&, vector<VkCommandBuffer>&);
