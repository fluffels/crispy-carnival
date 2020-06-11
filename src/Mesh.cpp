#pragma warning(disable: 4267)

#include <string>
#include <vector>

#include "easylogging++.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "Mesh.h"
#include "Vertex.h"

using std::string;
using std::vector;

using tinyobj::attrib_t;
using tinyobj::shape_t;
using tinyobj::LoadObj;

void loadObj(char* filename, attrib_t& attrib, vector<shape_t>& shapes) {
    string warn;
    string err;

    bool ret = LoadObj(
        &attrib, &shapes, nullptr, &warn, &err, filename
    );

    if (!warn.empty()) {
        LOG(WARNING) << warn;
    }

    if (!err.empty()) {
        LOG(ERROR) << err;
    }

    if (!ret) {
        exit(1);
    }
}

void uploadVertexData(Vulkan& vk, Mesh& mesh, attrib_t& attrib) {
    uint32_t count = attrib.vertices.size() / 3;
    vector<Vertex> vertices(count);
    for (uint32_t i = 0; i < count; i++) {
        uint32_t texIdx = 2 * i;
        uint32_t vertIdx = 3 * i;
        vertices[i].pos.x = attrib.vertices[vertIdx + 0];
        vertices[i].pos.y = attrib.vertices[vertIdx + 1];
        vertices[i].pos.z = attrib.vertices[vertIdx + 2];
        if (texIdx < attrib.texcoords.size()) {
            vertices[i].uv.x = attrib.texcoords[texIdx + 0];
            vertices[i].uv.y = attrib.texcoords[texIdx + 1];
        }
    }

    uint32_t size = sizeof(Vertex) * count;

    createVertexBuffer(
        vk.device, vk.memories, vk.queueFamily, size, mesh.vBuff
    );

    void* dst = mapMemory(vk.device, mesh.vBuff.handle, mesh.vBuff.memory);
        memcpy(dst, vertices.data(), size);
    unMapMemory(vk.device, mesh.vBuff.memory);
}

void uploadIndexData(Vulkan& vk, Mesh& mesh, vector<shape_t>& shapes) {
    vector<uint32_t> indices;
    for (auto& shape: shapes) {
        for (auto& index: shape.mesh.indices) {
            indices.push_back(index.vertex_index);
        }
    }

    mesh.idxCount = indices.size();
    uint32_t size = indices.size() * sizeof(uint32_t);

    createIndexBuffer(
        vk.device, vk.memories, vk.queueFamily, size, mesh.iBuff
    );

    void* dst = mapMemory(vk.device, mesh.iBuff.handle, mesh.iBuff.memory);
        memcpy(dst, indices.data(), size);
    unMapMemory(vk.device, mesh.iBuff.memory);
}

void uploadVertexDataFromObj(Vulkan& vk, char* filename, Mesh& mesh) {
    attrib_t attrib;
    vector<shape_t> shapes;
    loadObj(filename, attrib, shapes);
    uploadVertexData(vk, mesh, attrib);
    uploadIndexData(vk, mesh, shapes);
}
