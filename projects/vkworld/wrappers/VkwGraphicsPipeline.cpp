#include "VkwGraphicsPipeline.h"

#include <array>
#include <iostream>

#include <vulkan/vulkan.hpp>

#include "VkwMemoryHelper.h"

#include "Vulkan.h"

namespace world {

class VkwGraphicsPipelinePrivate {
public:
    // Parameters
    vk::RenderPass _renderPass;

    u32 _width = 0, _height = 0;

    bool _vertexBuffer = true;

    bool _dynamicViewport = false;

    vk::PrimitiveTopology _primitive = vk::PrimitiveTopology::eTriangleList;


    // Resources (destroyed with the pipeline)
    std::map<VkwShaderType, vk::ShaderModule> _shaders;

    vk::DescriptorSetLayout _descriptorSetLayout;

    vk::PipelineLayout _pipelineLayout;

    vk::Pipeline _pipeline;

    bool _initialized = false;

    ~VkwGraphicsPipelinePrivate() {
        if (!_initialized) {
            return;
        }

        auto &ctx = Vulkan::context();
        for (auto &entry : _shaders) {
            ctx._device.destroy(entry.second);
        }
        ctx._device.destroy(_pipelineLayout);
        ctx._device.destroy(_pipeline);
    }

    void checkParameters() {
        if (_width <= 0 || _height <= 0 || !_renderPass) {
            throw std::runtime_error("Bad pipeline parameters");
        }
    }

    void createPipeline() {
        checkParameters();
        auto &ctx = Vulkan::context();

        vk::PipelineLayoutCreateInfo pipelineLayoutInfo({}, 1,
                                                        &_descriptorSetLayout);
        _pipelineLayout = ctx._device.createPipelineLayout(pipelineLayoutInfo);

        // Create pipeline stage info for each shader
        std::vector<vk::PipelineShaderStageCreateInfo> stageInfos;

        if (_shaders.find(VkwShaderType::VERTEX) != _shaders.end()) {
            vk::PipelineShaderStageCreateInfo vertexStageInfo(
                {}, vk::ShaderStageFlagBits::eVertex,
                _shaders[VkwShaderType::VERTEX], "main");
            stageInfos.push_back(vertexStageInfo);
        }


        if (_shaders.find(VkwShaderType::FRAGMENT) != _shaders.end()) {
            vk::PipelineShaderStageCreateInfo fragmentStageInfo(
                {}, vk::ShaderStageFlagBits::eFragment,
                _shaders[VkwShaderType::FRAGMENT], "main");
            stageInfos.push_back(fragmentStageInfo);
        }

        // Fixed stages
        std::vector<vk::VertexInputBindingDescription> bindings;
        std::vector<vk::VertexInputAttributeDescription> attributes;

        vk::PipelineVertexInputStateCreateInfo vertInputStageInfo;

        if (_vertexBuffer) {
            bindings.emplace_back(0, sizeof(VkwVertex),
                                  vk::VertexInputRate::eVertex);

            // position, normal, uv
            attributes.emplace_back(0, 0, vk::Format::eR32G32B32Sfloat,
                                    offsetof(VkwVertex, _position));
            attributes.emplace_back(1, 0, vk::Format::eR32G32B32Sfloat,
                                    offsetof(VkwVertex, _normal));
            attributes.emplace_back(2, 0, vk::Format::eR32G32Sfloat,
                                    offsetof(VkwVertex, _uv));

            vertInputStageInfo.vertexBindingDescriptionCount = bindings.size();
            vertInputStageInfo.pVertexBindingDescriptions = &bindings[0];

            vertInputStageInfo.vertexAttributeDescriptionCount =
                attributes.size();
            vertInputStageInfo.pVertexAttributeDescriptions = &attributes[0];
        }

        vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStageInfo(
            {}, _primitive);

        vk::Viewport viewport(0, 0, _width, _height, 0, 1);
        vk::Rect2D scissor({0, 0}, {_width, _height});
        vk::PipelineViewportStateCreateInfo viewportStageInfo({}, 1, &viewport,
                                                              1, &scissor);

        // TODO: Add other parameters
        vk::PipelineRasterizationStateCreateInfo rasterizationStageInfo(
            {}, false, false, vk::PolygonMode::eFill);
        rasterizationStageInfo.lineWidth = 1;

        // TODO: Enable multisampling
        vk::PipelineMultisampleStateCreateInfo multisampleStageInfo;

        // TODO: Add depth and stencil test


        // TODO: enable color blending if the user wants to
        vk::PipelineColorBlendAttachmentState blendAttachment(
            false, vk::BlendFactor::eOne, vk::BlendFactor::eZero,
            vk::BlendOp::eAdd, vk::BlendFactor::eOne, vk::BlendFactor::eZero,
            vk::BlendOp::eAdd,
            vk::ColorComponentFlagBits::eA | vk::ColorComponentFlagBits::eR |
                vk::ColorComponentFlagBits::eG |
                vk::ColorComponentFlagBits::eB);

        vk::PipelineColorBlendStateCreateInfo blendStageInfo(
            {}, false, vk::LogicOp::eCopy, 1, &blendAttachment);

        // Create pipeline from layout and stage info
        vk::GraphicsPipelineCreateInfo pipelineCreateInfo({}, stageInfos.size(),
                                                          &stageInfos[0]);
        pipelineCreateInfo.layout = _pipelineLayout;
        pipelineCreateInfo.pVertexInputState = &vertInputStageInfo;
        pipelineCreateInfo.pInputAssemblyState = &inputAssemblyStageInfo;
        pipelineCreateInfo.pViewportState = &viewportStageInfo;
        pipelineCreateInfo.pRasterizationState = &rasterizationStageInfo;
        pipelineCreateInfo.pMultisampleState = &multisampleStageInfo;
        pipelineCreateInfo.pDepthStencilState =
            nullptr; // Optional but need a compatible renderpass (or segfault)
        pipelineCreateInfo.pColorBlendState = &blendStageInfo;
        pipelineCreateInfo.pDynamicState = nullptr; // Optional

        pipelineCreateInfo.renderPass = _renderPass;
        pipelineCreateInfo.subpass = 0;

        // TODO: Use this when available
        pipelineCreateInfo.basePipelineHandle = nullptr;
        pipelineCreateInfo.basePipelineIndex = -1;

        // TODO: May use pipeline cache?
        _pipeline =
            ctx._device.createGraphicsPipeline(nullptr, pipelineCreateInfo);
    }

    void initialize() {
        createPipeline();
        _initialized = true;
    }
};


VkwGraphicsPipeline::VkwGraphicsPipeline()
        : _internal(std::make_shared<VkwGraphicsPipelinePrivate>()) {}

VkwGraphicsPipeline::VkwGraphicsPipeline(
    VkwDescriptorSetLayout &descriptorSetLayout)
        : _internal(std::make_shared<VkwGraphicsPipelinePrivate>()) {

    _internal->_descriptorSetLayout = descriptorSetLayout.getLayout();
}

void VkwGraphicsPipeline::setLayout(VkwDescriptorSetLayout &dsetLayout) {
    _internal->_descriptorSetLayout = dsetLayout.getLayout();
}

void VkwGraphicsPipeline::enableVertexBuffer(bool enabled) {
    _internal->_vertexBuffer = enabled;
}

void VkwGraphicsPipeline::setPrimitive(vk::PrimitiveTopology &primitive) {
    _internal->_primitive = primitive;
}

void VkwGraphicsPipeline::setRenderPass(VkwRenderPass renderPass) {
    _internal->_renderPass = renderPass.handle();
    _internal->_width = renderPass.image().width();
    _internal->_height = renderPass.image().height();
}

void VkwGraphicsPipeline::setBuiltinShader(VkwShaderType type,
                                           const std::string &shaderName) {
    auto &ctx = Vulkan::context();
    _internal->_shaders[type] =
        ctx.createShader(ctx.readInternalFile(shaderName + ".spv"));
}

bool VkwGraphicsPipeline::hasDynamicViewport() const {
    return _internal->_dynamicViewport;
}

vk::Pipeline &VkwGraphicsPipeline::getPipeline() {
    if (!_internal->_initialized)
        _internal->initialize();
    return _internal->_pipeline;
}

vk::PipelineLayout &VkwGraphicsPipeline::getLayout() {
    if (!_internal->_initialized)
        _internal->initialize();
    return _internal->_pipelineLayout;
}
} // namespace world
