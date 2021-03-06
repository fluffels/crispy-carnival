#pragma warning(disable: 4267)

#include <string>
#include <vector>

#include "easylogging++.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "Mesh.h"
#include "Vertex.h"
#include "Vulkan.h"

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
    VulkanPipeline& pipeline,
    Obj& obj,
    Mesh& mesh
) {
    auto& attributes = pipeline.inputAttributes;
    auto stride = pipeline.inputBinding.stride;

    auto count = 0;
    for (auto& shape: obj.shapes) {
        count += shape.mesh.indices.size();
    }
    auto size = count * stride;
    float* vertices = (float*)malloc(size);
    float* vertex = vertices;

    for (auto& shape: obj.shapes) {
        // NOTE(jan): right now we assume the same material for the whole shape
        auto materialIdx = shape.mesh.material_ids[0];
        for (auto& index: shape.mesh.indices) {
            auto vertIndex = index.vertex_index * 3;
            auto colorIndex = vertIndex;
            auto texIndex = index.texcoord_index * 2;
            auto normalIndex = index.normal_index * 3;

            *vertex++ = obj.attrib.vertices[vertIndex + 0];
            *vertex++ = obj.attrib.vertices[vertIndex + 1];
            *vertex++ = obj.attrib.vertices[vertIndex + 2];
            if (pipeline.needsColor && (materialIdx != -1)) {
                auto& material = obj.materials[materialIdx];
                *vertex++ = material.diffuse[0];
                *vertex++ = material.diffuse[1];
                *vertex++ = material.diffuse[2];
            }
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
        vk.device, vk.memories, vk.queueFamily, size, mesh.vBuff
    );

    void* dst = mapMemory(vk.device, mesh.vBuff.handle, mesh.vBuff.memory);
        memcpy(dst, vertices, size);
    unMapMemory(vk.device, mesh.vBuff.memory);

    mesh.idxCount = count;

    free(vertices);
}

void uploadVertexDataFromObj(
    Vulkan& vk,
    VulkanPipeline& pipeline,
    char* filename,
    Mesh& mesh
) {
    Obj obj;
    loadObj(filename, obj);
    uploadVertexData(vk, pipeline, obj, mesh);
}
