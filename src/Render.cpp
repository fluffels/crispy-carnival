#pragma warning(disable: 4267)

#include "Render.h"

#include "Mesh.h"
#include "Texture.h"
#include "VulkanPipeline.h"

struct Brush {
    VulkanPipeline pipeline;
    Mesh mesh;
    VulkanSampler sampler;
};

void updateDescriptorSet(
    Vulkan& vk,
    VulkanPipeline& pipeline
) {
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
        write.dstSet = pipeline.descriptorSet;
        write.pBufferInfo = &mvpBufferInfo;
        writeSets.push_back(write);
    }

    vkUpdateDescriptorSets(
        vk.device,
        writeSets.size(), writeSets.data(),
        0,
        nullptr
    );
}

void updateDescriptorSet(
    Vulkan& vk,
    VulkanPipeline& pipeline,
    VulkanSampler& sampler
) {
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
        write.dstSet = pipeline.descriptorSet;
        write.pBufferInfo = &mvpBufferInfo;
        writeSets.push_back(write);
    }

    {
        VkDescriptorImageInfo imageInfo;
        imageInfo.imageView = sampler.image.view;
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.sampler = sampler.handle;

        VkWriteDescriptorSet write = {};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorCount = 1;
        write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write.dstBinding = writeSets.size();
        write.dstSet = pipeline.descriptorSet;
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

void recordCommandBuffers(Vulkan& vk, vector<VkCommandBuffer>& cmds) {
    Brush skybox;
    initVKPipeline(vk, "skybox", skybox.pipeline);
    uploadTextureCube(vk, skybox.sampler);
    uploadVertexDataFromObj(vk, skybox.pipeline, "models/skybox.obj", skybox.mesh);
    updateDescriptorSet(vk, skybox.pipeline, skybox.sampler);

    Brush spaceShip;
    initVKPipeline(vk, "spaceship", spaceShip.pipeline);
    uploadTexture(vk, "textures/spaceship.png", spaceShip.sampler);
    uploadVertexDataFromObj(
        vk,
        spaceShip.pipeline,
        "models/viper.obj",
        spaceShip.mesh
    );
    updateDescriptorSet(vk, spaceShip.pipeline, spaceShip.sampler);

    Brush planet;
    initVKPipeline(vk, "planet", planet.pipeline);
    uploadVertexDataFromObj(
        vk,
        planet.pipeline,
        "models/Planet_Sandy.obj",
        planet.mesh
    );
    updateDescriptorSet(vk, planet.pipeline);

    uint32_t framebufferCount = vk.swap.images.size();
    cmds.resize(framebufferCount);
    createCommandBuffers(vk.device, vk.cmdPool, framebufferCount, cmds);
    for (size_t swapIdx = 0; swapIdx < framebufferCount; swapIdx++) {
        auto& cmd = cmds[swapIdx];
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
        beginInfo.renderPass = vk.renderPass;

        vkCmdBeginRenderPass(cmd, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(
            cmd,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            skybox.pipeline.handle
        );
        vkCmdBindDescriptorSets(
            cmd,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            skybox.pipeline.layout,
            0,
            1,
            &skybox.pipeline.descriptorSet,
            0,
            nullptr
        );
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(
            cmd,
            0, 1,
            &skybox.mesh.vBuff.handle,
            offsets
        );
        vkCmdDraw(
            cmd,
            skybox.mesh.idxCount, 1,
            0, 0
        );

        vkCmdBindPipeline(
            cmd,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            spaceShip.pipeline.handle
        );
        vkCmdBindDescriptorSets(
            cmd,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            spaceShip.pipeline.layout,
            0,
            1,
            &spaceShip.pipeline.descriptorSet,
            0,
            nullptr
        );
        vkCmdBindVertexBuffers(
            cmd,
            0, 1,
            &spaceShip.mesh.vBuff.handle,
            offsets
        );
        vkCmdDraw(
            cmd,
            spaceShip.mesh.idxCount, 1,
            0, 0
        );

        vkCmdBindPipeline(
            cmd,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            planet.pipeline.handle
        );
        vkCmdBindDescriptorSets(
            cmd,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            planet.pipeline.layout,
            0,
            1,
            &planet.pipeline.descriptorSet,
            0,
            nullptr
        );
        vkCmdBindVertexBuffers(
            cmd,
            0, 1,
            &planet.mesh.vBuff.handle,
            offsets
        );
        vkCmdDraw(
            cmd,
            planet.mesh.idxCount, 1,
            0, 0
        );

        vkCmdEndRenderPass(cmd);

        checkSuccess(vkEndCommandBuffer(cmd));
    }
}
