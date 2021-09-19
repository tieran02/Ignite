#pragma once
#include "Ignite/Renderer/Pipeline.h"
#include <vulkan/vulkan_core.h>

namespace  Ignite
{
	class VulkanDevice;
	class VulkanContext;
	
	class VulkanPipeline : public Pipeline
	{
		friend class Pipeline;
	protected:
		VulkanPipeline(const PipelineCreateInfo& info);
	public:
		~VulkanPipeline() override;
	protected:
		void Init() override;
		void Cleanup() override;	
	public:
		void Free() override;
		void Recreate() override;
		void Bind() const override;
		void Unbind() const override;

		static const VkPipelineLayout& PipelineLayout();;
	private:
		static VkPipelineLayout m_pipelineLayout;
		VkPipeline m_pipeline;
		
		VkVertexInputBindingDescription  getBindingDescription();
		std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
		void createPipeline();
	};
}
