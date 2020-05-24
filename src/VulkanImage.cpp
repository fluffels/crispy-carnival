#include <stdexcept>

#include "VulkanImage.h"
#include "VulkanMemory.h"

using std::runtime_error;

VkDeviceMemory allocate(
    VkDevice device,
    VkDeviceSize size,
    uint32_t memoryType,
    VkImage image
) {
    VkMemoryAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize = size;
    allocateInfo.memoryTypeIndex = memoryType;
    
    VkDeviceMemory memory;
    auto result = vkAllocateMemory(
        device,
        &allocateInfo,
        nullptr,
        &memory
    );
    if (result != VK_SUCCESS) {
        throw runtime_error("could not allocate memory");
    }

    vkBindImageMemory(device, image, memory, 0);

    return memory;
}

VkImage createImage(
    VkDevice device,
    VkImageType type,
    VkExtent2D extent,
    uint32_t layerCount,
    uint32_t family,
    VkFormat format,
    VkImageUsageFlags usage,
    VkImageCreateFlags flags
) {
    VkImage result;

    VkImageCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    createInfo.flags = flags;
    createInfo.imageType = VK_IMAGE_TYPE_2D;
    createInfo.extent = { extent.width, extent.height, 1 };
    createInfo.mipLevels = 1;
    createInfo.arrayLayers = layerCount;
    createInfo.format = format;
    createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    createInfo.queueFamilyIndexCount = 1;
    createInfo.pQueueFamilyIndices = &family;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.usage = usage;
    createInfo.samples = VK_SAMPLE_COUNT_1_BIT;

    auto code = vkCreateImage(device, &createInfo, nullptr, &result);
    if (code != VK_SUCCESS) {
        throw runtime_error("could not create image");
    }

    return result;
}

VkImageView createView(
    VkDevice device,
    VkImage image,
    VkImageViewType type,
    VkFormat format,
    VkImageAspectFlags aspectMask
) {
    VkImageView result;

    VkImageViewCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.viewType = type;
    createInfo.format = format;
    createInfo.image = image;
    createInfo.subresourceRange.aspectMask = aspectMask;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
    createInfo.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;

    auto code = vkCreateImageView(device, &createInfo, nullptr, &result);
    if (code != VK_SUCCESS) {
        throw runtime_error("could not create image view");  
    }

    return result;
}

VulkanImage createVulkanImage(
    VkDevice device,
    VkPhysicalDeviceMemoryProperties& memories,
    VkImageType type,
    VkImageViewType viewType,
    VkExtent2D extent,
    uint32_t layerCount,
    uint32_t family,
    VkFormat format,
    VkImageUsageFlags usage,
    VkImageAspectFlags aspectMask,
    bool hostVisible = false,
    VkImageCreateFlags imageCreateFlags = 0
) {
    VulkanImage result = {};

    result.handle = createImage(
        device,
        type,
        extent,
        layerCount,
        family,
        format,
        usage,
        imageCreateFlags
    );

    auto reqs = getMemoryRequirements(device, result.handle);
    auto flags =
        hostVisible? VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT : 0;
    auto memType = selectMemoryTypeIndex(memories, reqs, flags);
    result.memory = allocate(device, reqs.size, memType, result.handle);

    result.view = createView(
        device, result.handle, viewType, format, aspectMask
    );

    return result;
}

void destroyVulkanImage(VkDevice device, VulkanImage image) {
    vkDestroyImageView(device, image.view, nullptr);
    vkFreeMemory(device, image.memory, nullptr);
    vkDestroyImage(device, image.handle, nullptr);
}

void destroyVulkanSampler(VkDevice device, VulkanSampler sampler) {
    vkDestroySampler(device, sampler.handle, nullptr);
    destroyVulkanImage(device, sampler.image);
}

VulkanImage createVulkanDepthBuffer(
    VkDevice device,
    VkPhysicalDeviceMemoryProperties& memories,
    VkExtent2D extent,
    uint32_t family
) {
    auto result = createVulkanImage(
        device,
        memories,
        VK_IMAGE_TYPE_2D,
        VK_IMAGE_VIEW_TYPE_2D,
        extent,
        1,
        family,
        VK_FORMAT_D32_SFLOAT,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_IMAGE_ASPECT_DEPTH_BIT
    );
    return result;
}

VulkanSampler createVulkanSampler(
    VkDevice device,
    VkPhysicalDeviceMemoryProperties& memories,
    VkImageType type,
    VkImageViewType viewType,
    VkExtent2D extent,
    uint32_t layerCount,
    uint32_t family,
    VkImageCreateFlags imageCreateFlags = (VkImageCreateFlags)0
) {
    VulkanSampler result = {};

    result.image = createVulkanImage(
        device,
        memories,
        type,
        viewType,
        extent,
        layerCount,
        family,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        VK_IMAGE_ASPECT_COLOR_BIT,
        false,
        imageCreateFlags
    );

    VkSamplerCreateInfo createInfo = { VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
    createInfo.magFilter = VK_FILTER_LINEAR;
    createInfo.minFilter = VK_FILTER_LINEAR;
    createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
    createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    // TODO(jan): Enable anisotropic filtering.
    createInfo.anisotropyEnable = VK_FALSE;
    // createInfo.maxAnisotropy = ;
    createInfo.compareEnable = VK_FALSE;
    // createInfo.minLod = ;
    // createInfo.maxLod = ;
    // createInfo.mipLodBias = ;

    auto code = vkCreateSampler(device, &createInfo, nullptr, &result.handle);
    if (code != VK_SUCCESS) {
        throw runtime_error("could not create sampler");
    }

    return result;
}

VulkanSampler createVulkanSampler2D(
    VkDevice device,
    VkPhysicalDeviceMemoryProperties& memories,
    VkExtent2D extent,
    uint32_t family
) {
    return createVulkanSampler(
        device,
        memories,
        VK_IMAGE_TYPE_2D,
        VK_IMAGE_VIEW_TYPE_2D,
        extent,
        1,
        family
    );
}

VulkanSampler createVulkanSamplerCube(
    VkDevice device,
    VkPhysicalDeviceMemoryProperties& memories,
    VkExtent2D extent,
    uint32_t family
) {
    VulkanImage result = {};

    return createVulkanSampler(
        device,
        memories,
        VK_IMAGE_TYPE_2D,
        VK_IMAGE_VIEW_TYPE_CUBE,
        extent,
        6,
        family,
        VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT
    );
}
