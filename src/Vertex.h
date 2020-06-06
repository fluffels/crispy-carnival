#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

class Vertex {
    public:
        glm::vec3 pos;
        glm::vec3 uv;

        static VkVertexInputBindingDescription
        getInputBindingDescription();

        static std::vector<VkVertexInputAttributeDescription>
        getInputAttributeDescriptions();
};
