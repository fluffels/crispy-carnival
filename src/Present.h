#pragma once

#include "RenderPass.h"
#include "Vulkan.h"

void present(Vulkan& vk, RenderPass& pass);
void updateMVP(Vulkan& vk, void* data, size_t length);

