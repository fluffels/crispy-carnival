#pragma warning(disable: 4267)
#pragma warning(disable: 4838)

#include <string>
#include <vector>

#include "Brush.h"
#include "FileSystem.h"
#include "Spaceship.h"
#include "Texture.h"
#include "util.h"
#include "Vertex.h"
#include "VulkanPipeline.h"

using std::string;
using std::vector;

void updateDescriptorSetSpaceShip(Vulkan& vk, VulkanPipeline& pipeline, VulkanSampler& sampler) {
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

void initSpaceship(Vulkan& vk, Brush& brush) {
    initVKPipeline(vk, "spaceship", brush.pipeline);
    VulkanSampler sampler;
    uploadTexture(vk, "textures/spaceship.png", sampler);
    uploadVertexDataFromObj(vk, brush, "models/viper.obj");
    updateDescriptorSetSpaceShip(vk, brush.pipeline, sampler);
}
