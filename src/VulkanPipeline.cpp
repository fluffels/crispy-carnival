#include "util.h"
#include "FileSystem.h"
#include "Vertex.h"
#include "Vulkan.h"

void createDescriptorLayout(Vulkan& vk) {
    vector<VkDescriptorSetLayoutBinding> bindings;

    VkDescriptorSetLayoutBinding binding = {};
    binding.binding = 0;
    binding.descriptorCount = 1;
    binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    bindings.push_back(binding);
    
    binding = {};
    binding.binding = 1;
    binding.descriptorCount = 1;
    binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    bindings.push_back(binding);

    VkDescriptorSetLayoutCreateInfo descriptors = {};
    descriptors.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptors.bindingCount = (uint32_t)bindings.size();
    descriptors.pBindings = bindings.data();
    
    checkSuccess(vkCreateDescriptorSetLayout(
        vk.device,
        &descriptors,
        nullptr,
        &vk.pipeline.descriptorLayout
    ));
}

void createDescriptorPool(Vulkan& vk) {
    VkDescriptorPoolSize poolSize = {};
    poolSize.descriptorCount = 1;
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

    VkDescriptorPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    createInfo.maxSets = 1;
    createInfo.poolSizeCount = 1;
    createInfo.pPoolSizes = &poolSize;

    checkSuccess(vkCreateDescriptorPool(
        vk.device,
        &createInfo,
        nullptr,
        &vk.pipeline.descriptorPool
    ));
}

void allocateDescriptorSet(Vulkan& vk) {
    VkDescriptorSetAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.descriptorPool = vk.pipeline.descriptorPool;
    allocateInfo.descriptorSetCount = 1;
    allocateInfo.pSetLayouts = &vk.pipeline.descriptorLayout;
    checkSuccess(vkAllocateDescriptorSets(
        vk.device,
        &allocateInfo,
        &vk.pipeline.descriptorSet
    ));
}

void createRenderPass(Vulkan& vk) {
    vector<VkAttachmentDescription> attachments;
    VkAttachmentDescription color = {};
    color.format = vk.swap.format;
    color.samples = VK_SAMPLE_COUNT_1_BIT;
    color.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    attachments.push_back(color);
    
    VkAttachmentDescription depth = {};
    depth.format = VK_FORMAT_D32_SFLOAT;
    depth.samples = VK_SAMPLE_COUNT_1_BIT;
    depth.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depth.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depth.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    attachments.push_back(depth);

    vector<VkAttachmentReference> colorReferences;
    VkAttachmentReference colorReference = {};
    colorReference.attachment = 0;
    colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorReferences.push_back(colorReference);

    VkAttachmentReference depthReference = {};
    depthReference.attachment = 1;
    depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    vector<VkSubpassDescription> subpasses;
    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = (uint32_t)colorReferences.size();
    subpass.pColorAttachments = colorReferences.data();
    subpass.pDepthStencilAttachment = &depthReference;
    subpasses.push_back(subpass);

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.attachmentCount = (uint32_t)attachments.size();
    createInfo.pAttachments = attachments.data();
    createInfo.subpassCount = (uint32_t)subpasses.size();
    createInfo.pSubpasses = subpasses.data();
    createInfo.dependencyCount = 1;
    createInfo.pDependencies = &dependency;

    checkSuccess(vkCreateRenderPass(
        vk.device, &createInfo, nullptr, &vk.pipeline.renderPass
    ));
}

VkShaderModule createShaderModule(Vulkan& vk, const vector<char>& code) {
    VkShaderModule shaderModule;

    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
    checkSuccess(vkCreateShaderModule(
        vk.device,
        &createInfo,
        nullptr,
        &shaderModule
    ));
    return shaderModule;
}

VkShaderModule createShaderModule(Vulkan& vk, const string& path) {
    auto code = readFile(path);
    return createShaderModule(vk, code);
}

void createPipelineLayout(Vulkan& vk) {
    VkPipelineLayoutCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    createInfo.setLayoutCount = 1;
    createInfo.pSetLayouts = &vk.pipeline.descriptorLayout;
    checkSuccess(vkCreatePipelineLayout(
        vk.device,
        &createInfo,
        nullptr,
        &vk.pipeline.layout
    ));
}

void createPipeline(
    Vulkan& vk,
    VkShaderModule& vertModule,
    VkShaderModule& fragModule
) {
    vector<VkPipelineShaderStageCreateInfo> shaderStages;
    if (vertModule != nullptr) {
        VkPipelineShaderStageCreateInfo vertStage = {};
        vertStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertStage.module = vertModule;
        vertStage.pName = "main";
        shaderStages.push_back(vertStage);
    }
    if (fragModule != nullptr) {
        VkPipelineShaderStageCreateInfo fragStage = {};
        fragStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragStage.module = fragModule;
        fragStage.pName = "main";
        shaderStages.push_back(fragStage);
    }

    auto inputBinding = Vertex::getInputBindingDescription();
    auto inputAttributes = Vertex::getInputAttributeDescriptions();

    VkPipelineVertexInputStateCreateInfo vertexInput = {};
    vertexInput.sType =
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInput.vertexBindingDescriptionCount = 1;
    vertexInput.pVertexBindingDescriptions = &inputBinding;
    vertexInput.vertexAttributeDescriptionCount =
        (uint32_t)inputAttributes.size();
    vertexInput.pVertexAttributeDescriptions = inputAttributes.data();
    
    VkPipelineInputAssemblyStateCreateInfo assembly = {};
    assembly.sType =
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    assembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport = {};
    viewport.height = (float)vk.swap.extent.height;
    viewport.width = (float)vk.swap.extent.width;
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f; 
    viewport.x = 0.f;
    viewport.y = 0.f;

    VkRect2D scissor = {};
    scissor.offset = {0, 0};
    scissor.extent = vk.swap.extent;

    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo raster = {};
    raster.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    raster.frontFace = VK_FRONT_FACE_CLOCKWISE;
    raster.cullMode = VK_CULL_MODE_BACK_BIT;
    raster.lineWidth = 1.f;
    raster.polygonMode = VK_POLYGON_MODE_FILL;
    raster.rasterizerDiscardEnable = VK_FALSE;
    raster.depthClampEnable = VK_FALSE;
    raster.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo msample = {};
    msample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    msample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    msample.sampleShadingEnable = VK_FALSE;
    msample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    msample.minSampleShading = 1.0f;
    msample.pSampleMask = nullptr;
    msample.alphaToCoverageEnable = VK_FALSE;
    msample.alphaToOneEnable = VK_FALSE;

    VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo = {};
    depthStencilCreateInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilCreateInfo.depthTestEnable = VK_TRUE;
    depthStencilCreateInfo.depthWriteEnable = VK_TRUE;
    depthStencilCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    // TODO(jan): Experiment with enabling this for better performance.
    depthStencilCreateInfo.depthBoundsTestEnable = VK_FALSE;
    depthStencilCreateInfo.stencilTestEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState colorBlend = {};
    colorBlend.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                                VK_COLOR_COMPONENT_G_BIT |
                                VK_COLOR_COMPONENT_B_BIT |
                                VK_COLOR_COMPONENT_A_BIT;
    colorBlend.blendEnable = VK_FALSE;
    colorBlend.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlend.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlend.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlend.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlend.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlend.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo blending = {};
    blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    blending.logicOpEnable = VK_FALSE;
    blending.logicOp = VK_LOGIC_OP_COPY;
    blending.attachmentCount = 1;
    blending.pAttachments = &colorBlend;
    blending.blendConstants[0] = 0.0f;
    blending.blendConstants[1] = 0.0f;
    blending.blendConstants[2] = 0.0f;
    blending.blendConstants[3] = 0.0f;

    VkGraphicsPipelineCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    createInfo.stageCount = (uint32_t)shaderStages.size();
    createInfo.pStages = shaderStages.data();
    createInfo.pVertexInputState = &vertexInput;
    createInfo.pInputAssemblyState = &assembly;
    createInfo.pViewportState = &viewportState;
    createInfo.pRasterizationState = &raster;
    createInfo.pMultisampleState = &msample;
    createInfo.pColorBlendState = &blending;
    createInfo.pDepthStencilState = &depthStencilCreateInfo;
    createInfo.renderPass = vk.pipeline.renderPass;
    createInfo.layout = vk.pipeline.layout;
    createInfo.subpass = 0;
    
    checkSuccess(vkCreateGraphicsPipelines(
        vk.device,
        VK_NULL_HANDLE,
        1,
        &createInfo,
        nullptr,
        &vk.pipeline.handle
    ));
}

void initVKPipeline(Vulkan& vk) {
    createDescriptorLayout(vk);
    createDescriptorPool(vk);
    allocateDescriptorSet(vk);
    createRenderPass(vk);
    createPipelineLayout(vk);
    auto vert = createShaderModule(vk, "shaders/skybox.vert.spv");
    auto frag = createShaderModule(vk, "shaders/skybox.frag.spv");
    createPipeline(vk, vert, frag);
}
