#ifndef VKWORLD_COMPUTEPIPELINE_H
#define VKWORLD_COMPUTEPIPELINE_H

#include "VkWorldConfig.h"

#include <memory>

#include <world/core/WorldTypes.h>

#include "VkEnums.h"
#include "DescriptorSetLayoutVk.h"

namespace vk {
class Pipeline;
class PipelineLayout;
} // namespace vk

namespace world {

class VkwComputePipelinePrivate;

class VKWORLD_EXPORT VkwComputePipeline {
public:
    VkwComputePipeline(VkwDescriptorSetLayout &descriptorSetLayout);

    VkwComputePipeline(VkwDescriptorSetLayout &descriptorSetLayout,
                       const std::string &shaderName);

    void setBuiltinShader(const std::string &shaderName);

    /** Returns the pipeline created with current configuration of this
     * ComputePipeline. */
    vk::Pipeline &getPipeline();

    vk::PipelineLayout &getLayout();

private:
    std::shared_ptr<VkwComputePipelinePrivate> _internal;
};
} // namespace world

#endif // VKWORLD_COMPUTEPIPELINE_H