#pragma once

#include "Brush.h"
#include "Vulkan.h"

void present(Vulkan& vk, Brush& pass);
void updateMVP(Vulkan& vk, void* data, size_t length);

