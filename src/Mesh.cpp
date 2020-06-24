#pragma warning(disable: 4267)

#include <string>
#include <vector>

#include "easylogging++.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "Brush.h"
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
    Brush& brush,
    Obj& obj
) {
    auto& pipeline = brush.pipeline;
    auto& attributes = pipeline.inputAttributes;
    auto stride = pipeline.inputBinding.stride;

    auto count = 0;
    for (auto& shape: obj.shapes) {
        auto& mesh = shape.mesh;
        count += mesh.indices.size();
    }
    auto size = count * stride;
    float* vertices = (float*)malloc(size);
    float* vertex = vertices;

    for (auto& shape: obj.shapes) {
        auto& mesh = shape.mesh;
        
        for (auto& index: mesh.indices) {
            auto vertIndex = index.vertex_index * 3;
            auto texIndex = index.texcoord_index * 2;
            auto normalIndex = index.normal_index * 3;

            *vertex++ = obj.attrib.vertices[vertIndex + 0];
            *vertex++ = obj.attrib.vertices[vertIndex + 1];
            *vertex++ = obj.attrib.vertices[vertIndex + 2];
            if (pipeline.needsTexCoords &&
                    (texIndex < obj.attrib.texcoords.size())) {
                *vertex++ = obj.attrib.texcoords[texIndex + 0];
                *vertex++ = obj.attrib.texcoords[texIndex + 1];
            }
            if (pipeline.needsNormals &&
                    (normalIndex < obj.attrib.normals.size())) {
                *vertex++ = obj.attrib.normals[normalIndex + 0];
                *vertex++ = obj.attrib.normals[normalIndex + 1];
                *vertex++ = obj.attrib.normals[normalIndex + 2];
            }
        }
    }

    createVertexBuffer(
        vk.device, vk.memories, vk.queueFamily, size, brush.mesh.vBuff
    );

    void* dst = mapMemory(vk.device, brush.mesh.vBuff.handle, brush.mesh.vBuff.memory);
        memcpy(dst, vertices, size);
    unMapMemory(vk.device, brush.mesh.vBuff.memory);

    brush.mesh.idxCount = count;

    free(vertices);
}

void uploadVertexDataFromObj(
    Vulkan& vk,
    Brush& brush,
    char* filename
) {
    Obj obj;
    loadObj(filename, obj);
    uploadVertexData(vk, brush, obj);
}
