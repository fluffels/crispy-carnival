#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "glm/glm/vec2.hpp"
#include "glm/glm/vec3.hpp"

class Vertex {
    public:
        glm::vec3 pos;
        glm::vec2 uv;
        glm::vec3 normal;

        static VkVertexInputBindingDescription
        getInputBindingDescription();

        static std::vector<VkVertexInputAttributeDescription>
        getInputAttributeDescriptions();
};
