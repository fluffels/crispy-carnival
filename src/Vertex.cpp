#pragma warning(disable: 4267)

#include "Vertex.h"

VkVertexInputBindingDescription Vertex::
getInputBindingDescription() {
    VkVertexInputBindingDescription i = {};
    i.binding = 0;
    i.stride = sizeof(Vertex);
    i.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return i;
}

std::vector<VkVertexInputAttributeDescription> Vertex::
getInputAttributeDescriptions() {
    std::vector<VkVertexInputAttributeDescription> attrs;

    VkVertexInputAttributeDescription attr = {};
    attr.binding = 0;
    attr.location = attrs.size();
    attr.format = VK_FORMAT_R32G32B32_SFLOAT;
    attr.offset = offsetof(Vertex, pos);
    attrs.push_back(attr);

    attr.binding = 0;
    attr.location = attrs.size();
    attr.format = VK_FORMAT_R32G32_SFLOAT;
    attr.offset = offsetof(Vertex, texCoord);
    attrs.push_back(attr);

    attr.binding = 0;
    attr.location = attrs.size();
    attr.format = VK_FORMAT_R32_UINT;
    attr.offset = offsetof(Vertex, texIdx);
    attrs.push_back(attr);

    return attrs;
}
