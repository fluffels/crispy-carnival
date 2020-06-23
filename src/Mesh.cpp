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
using tinyobj::material_t;
using tinyobj::shape_t;
using tinyobj::LoadObj;

struct Obj {
    attrib_t attrib;
    vector<shape_t> shapes;
    vector<material_t> materials;
};

void loadObj(
    char* filename,
    Obj& obj
) {
    string warn;
    string err;

    bool ret = LoadObj(
        &obj.attrib,
        &obj.shapes,
        &obj.materials,
        &warn, &err,
        filename, "models/"
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

void uploadVertexData(
    Vulkan& vk,
    Mesh& mesh,
    Obj& obj
) {
    vector<Vertex> vertices;

    vector<uint32_t> indices;
    for (auto& shape: obj.shapes) {
        auto& mesh = shape.mesh;

        for (auto& index: mesh.indices) {
            auto& vertex = vertices.emplace_back();

            auto vertIndex = index.vertex_index * 3;
            auto texIndex = index.texcoord_index * 2;
            auto normalIndex = index.normal_index * 3;

            vertex.pos.x = obj.attrib.vertices[vertIndex + 0];
            vertex.pos.y = obj.attrib.vertices[vertIndex + 1];
            vertex.pos.z = obj.attrib.vertices[vertIndex + 2];
            if (texIndex < obj.attrib.texcoords.size()) {
                vertex.uv.s = obj.attrib.texcoords[texIndex + 0];
                vertex.uv.t = obj.attrib.texcoords[texIndex + 1];
            }
            if (normalIndex < obj.attrib.normals.size()) {
                vertex.normal.x = obj.attrib.normals[normalIndex + 0];
                vertex.normal.y = obj.attrib.normals[normalIndex + 1];
                vertex.normal.z = obj.attrib.normals[normalIndex + 2];
            }
        }
    }

    uint32_t size = sizeof(Vertex) * vertices.size();

    createVertexBuffer(
        vk.device, vk.memories, vk.queueFamily, size, mesh.vBuff
    );

    void* dst = mapMemory(vk.device, mesh.vBuff.handle, mesh.vBuff.memory);
        memcpy(dst, vertices.data(), size);
    unMapMemory(vk.device, mesh.vBuff.memory);

    mesh.idxCount = vertices.size();
}

void uploadVertexDataFromObj(Vulkan& vk, char* filename, Mesh& mesh) {
    Obj obj;
    loadObj(filename, obj);
    uploadVertexData(vk, mesh, obj);
}
