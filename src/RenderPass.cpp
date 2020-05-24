#pragma warning(disable: 4267)
#pragma warning(disable: 4838)

#include <string>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "RenderPass.h"
#include "util.h"
#include "Vertex.h"

using std::string;
using std::vector;

void createCommandBuffers(Vulkan& vk, RenderPass& pass) {
    auto count = vk.swap.images.size();
    pass.cmds.resize(count);

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = vk.cmdPool;
    allocInfo.commandBufferCount = (uint32_t)count;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    checkSuccess(vkAllocateCommandBuffers(
        vk.device,
        &allocInfo,
        pass.cmds.data()
    ));
}

void recordCommandBuffers(Vulkan& vk, RenderPass& pass) {
    for (size_t swapIdx = 0; swapIdx < vk.swap.images.size(); swapIdx++) {
        auto cmd = pass.cmds[swapIdx];
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
            &pass.vBuff.handle,
            offsets
        );
        vkCmdBindIndexBuffer(
            cmd,
            pass.iBuff.handle,
            0,
            VK_INDEX_TYPE_UINT32
        );
        vkCmdDrawIndexed(
            cmd,
            pass.idxCount, 1,
            0, 0,
            0
        );

        vkCmdEndRenderPass(cmd);

        checkSuccess(vkEndCommandBuffer(cmd));
    }
}

void updateDescriptorSet(Vulkan& vk, RenderPass& pass) {
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
        imageInfo.imageView = pass.skybox.image.view;
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.sampler = pass.skybox.handle;

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

void uploadTextures(Vulkan& vk, RenderPass& pass) {
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
    pass.skybox = createVulkanSamplerCube(
        vk.device, vk.memories, extent, vk.queueFamily
    );
    auto& image = pass.skybox.image;

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

void uploadVertexData(Vulkan& vk, RenderPass& pass) {
    const float SIZE = 500.f;

    vector<Vertex> vertices(8);

    vertices[0].pos = { -SIZE, -SIZE, SIZE };
    vertices[1].pos = { -SIZE, SIZE, SIZE };
    vertices[2].pos = { SIZE, SIZE, SIZE };
    vertices[3].pos = { SIZE, -SIZE, SIZE };

    vertices[4].pos = { -SIZE, -SIZE, -SIZE };
    vertices[5].pos = { -SIZE, SIZE, -SIZE };
    vertices[6].pos = { SIZE, SIZE, -SIZE };
    vertices[7].pos = { SIZE, -SIZE, -SIZE };

    uint32_t size = sizeof(Vertex) * vertices.size();

    createVertexBuffer(
        vk.device, vk.memories, vk.queueFamily, size, pass.vBuff
    );

    void* dst = mapMemory(vk.device, pass.vBuff.handle, pass.vBuff.memory);
        memcpy(dst, vertices.data(), size);
    unMapMemory(vk.device, pass.vBuff.memory);
}

void uploadIndexData(Vulkan& vk, RenderPass& pass) {
    uint32_t indices[] = {
        0, 1, 2, 3, 7, 4, 5, 1,
        0xFFFFFFFF,
        6, 2, 1, 5, 4, 7, 3, 2
    };

    uint32_t size = sizeof(indices);
    pass.idxCount = size / sizeof(uint32_t);

    createIndexBuffer(
        vk.device, vk.memories, vk.queueFamily, size, pass.iBuff
    );

    void* dst = mapMemory(vk.device, pass.iBuff.handle, pass.iBuff.memory);
        memcpy(dst, indices, size);
    unMapMemory(vk.device, pass.iBuff.memory);
}

void initRenderPass(Vulkan& vk, RenderPass& pass) {
    uploadTextures(vk, pass);
    updateDescriptorSet(vk, pass);
    uploadVertexData(vk, pass);
    uploadIndexData(vk, pass);
    createCommandBuffers(vk, pass);
    recordCommandBuffers(vk, pass);
}
