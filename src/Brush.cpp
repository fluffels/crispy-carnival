#pragma warning(disable: 4267)
#pragma warning(disable: 4838)

#include <string>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "FileSystem.h"
#include "Brush.h"
#include "util.h"
#include "Vertex.h"

using std::string;
using std::vector;
using tinyobj::attrib_t;
using tinyobj::shape_t;
using tinyobj::LoadObj;

void createCommandBuffers(Vulkan& vk, Brush& brush) {
    auto count = vk.swap.images.size();
    brush.cmds.resize(count);

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = vk.cmdPool;
    allocInfo.commandBufferCount = (uint32_t)count;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    checkSuccess(vkAllocateCommandBuffers(
        vk.device,
        &allocInfo,
        brush.cmds.data()
    ));
}

void recordCommandBuffers(Vulkan& vk, Brush& brush) {
    for (size_t swapIdx = 0; swapIdx < vk.swap.images.size(); swapIdx++) {
        auto cmd = brush.cmds[swapIdx];
        beginFrameCommandBuffer(cmd);

        VkClearValue colorClear;
        colorClear.color = {1.f, 1.f, 1.f, 1.f};
        VkClearValue depthClear;
        depthClear.depthStencil = { 1.f, 0 };
        VkClearValue clears[] = { colorClear, depthClear };

        VkRenderPassBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        beginInfo.clearValueCount = 2;
        beginInfo.pClearValues = clears;
        beginInfo.framebuffer = vk.swap.framebuffers[swapIdx];
        beginInfo.renderArea.extent = vk.swap.extent;
        beginInfo.renderArea.offset = {0, 0};
        beginInfo.renderPass = vk.pipeline.renderPass;

        vkCmdBeginRenderPass(cmd, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(
            cmd,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            vk.pipeline.handle
        );

        vkCmdBindDescriptorSets(
            cmd,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            vk.pipeline.layout,
            0,
            1,
            &vk.pipeline.descriptorSet,
            0,
            nullptr
        );

        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(
            cmd,
            0, 1,
            &brush.mesh.vBuff.handle,
            offsets
        );
        vkCmdBindIndexBuffer(
            cmd,
            brush.mesh.iBuff.handle,
            0,
            VK_INDEX_TYPE_UINT32
        );
        vkCmdDrawIndexed(
            cmd,
            brush.mesh.idxCount, 1,
            0, 0,
            0
        );

        vkCmdBindVertexBuffers(
            cmd,
            0, 1,
            &brush.mesh2.vBuff.handle,
            offsets
        );
        vkCmdBindIndexBuffer(
            cmd,
            brush.mesh2.iBuff.handle,
            0,
            VK_INDEX_TYPE_UINT32
        );
        vkCmdDrawIndexed(
            cmd,
            brush.mesh2.idxCount, 1,
            0, 0,
            0
        );

        vkCmdEndRenderPass(cmd);

        checkSuccess(vkEndCommandBuffer(cmd));
    }
}

void updateDescriptorSet(Vulkan& vk, Brush& brush) {
    VkDescriptorBufferInfo mvpBufferInfo;
    mvpBufferInfo.buffer = vk.mvp.handle;
    mvpBufferInfo.offset = 0;
    mvpBufferInfo.range = VK_WHOLE_SIZE;

    vector<VkWriteDescriptorSet> writeSets;

    {
        VkWriteDescriptorSet write = {};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorCount = 1;
        write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write.dstBinding = writeSets.size();
        write.dstSet = vk.pipeline.descriptorSet;
        write.pBufferInfo = &mvpBufferInfo;
        writeSets.push_back(write);
    }

    {
        VkDescriptorImageInfo imageInfo;
        imageInfo.imageView = brush.skybox.image.view;
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.sampler = brush.skybox.handle;

        VkWriteDescriptorSet write = {};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorCount = 1;
        write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write.dstBinding = writeSets.size();
        write.dstSet = vk.pipeline.descriptorSet;
        write.pImageInfo = &imageInfo;
        writeSets.push_back(write);
    }

    vkUpdateDescriptorSets(
        vk.device,
        writeSets.size(), writeSets.data(),
        0,
        nullptr
    );
}

void uploadTextures(Vulkan& vk, Brush& brush) {
    // NOTE(jan): Extent must be the same for all cube face textures.
    VkExtent2D extent;

    vector<string> fnames = {
        "textures/xn.png",
        "textures/xp.png",
        "textures/yn.png",
        "textures/yp.png",
        "textures/zn.png",
        "textures/zp.png",
    };
    vector<VulkanBuffer> stagingBuffers;
    for (auto& fname: fnames) {
        int32_t x, y, n;
        uint8_t* data = stbi_load(fname.c_str(), &x, &y, &n, 4);
        extent = {(uint32_t)x, (uint32_t)y};
        auto size = x * y * 4;

        auto& staging = stagingBuffers.emplace_back();
        createStagingBuffer(
            vk.device,
            vk.memories,
            vk.queueFamily,
            size,
            staging
        );

        void* dst = mapMemory(vk.device, staging.handle, staging.memory);
            memcpy(dst, data, size);
        unMapMemory(vk.device, staging.memory);
    }
    brush.skybox = createVulkanSamplerCube(
        vk.device, vk.memories, extent, vk.queueFamily
    );
    auto& image = brush.skybox.image;

    auto cmd = allocateCommandBuffer(vk.device, vk.cmdPoolTransient);
    beginOneOffCommandBuffer(cmd);

    {
        VkImageMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.image = image.handle;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        vkCmdPipelineBarrier(
            cmd,
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );
    }

    for (int layerIdx = 0; layerIdx < stagingBuffers.size(); layerIdx++) {
        auto& staging = stagingBuffers[layerIdx];

        VkBufferImageCopy region = {};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.baseArrayLayer = layerIdx;
        region.imageSubresource.layerCount = 1;
        region.imageSubresource.mipLevel = 0;
        region.imageOffset = { 0, 0, 0 };
        region.imageExtent = { extent.width, extent.height, 1 };

        vkCmdCopyBufferToImage(
            cmd,
            staging.handle,
            image.handle,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1, &region
        );
    }

    {
        VkImageMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.image = image.handle;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(
            cmd,
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );
    }

    endCommandBuffer(cmd);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmd;
    vkQueueSubmit(vk.queue, 1, &submitInfo, nullptr);
}

void loadObj(char* filename, attrib_t& attrib, vector<shape_t>& shapes) {
    std::string warn;
    std::string err;

    bool ret = LoadObj(
        &attrib, &shapes, nullptr, &warn, &err, filename
    );

    if (!warn.empty()) {
    std::cout << warn << std::endl;
    }

    if (!err.empty()) {
    std::cerr << err << std::endl;
    }

    if (!ret) {
    exit(1);
    }
}

void uploadVertexData(Vulkan& vk, Mesh& mesh, attrib_t& attrib) {
    uint32_t count = attrib.vertices.size() / 3;
    vector<Vertex> vertices(count);
    for (uint32_t i = 0; i < count; i++) {
        uint32_t vertIdx = 3 * i;
        vertices[i].pos.x = attrib.vertices[vertIdx + 0];
        vertices[i].pos.y = attrib.vertices[vertIdx + 1];
        vertices[i].pos.z = attrib.vertices[vertIdx + 2];
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

void initBrush(Vulkan& vk, Brush& brush) {
    uploadVertexDataFromObj(vk, "models/skybox.obj", brush.mesh);
    uploadVertexDataFromObj(vk, "models/viper.obj", brush.mesh2);
    uploadTextures(vk, brush);
    updateDescriptorSet(vk, brush);
    createCommandBuffers(vk, brush);
    recordCommandBuffers(vk, brush);
}
