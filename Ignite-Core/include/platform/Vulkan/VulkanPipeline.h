#pragma once
#include "Ignite/Renderer/IPipeline.h"
#include <vulkan/vulkan_core.h>

namespace  Ignite
{
	class VulkanDevice;
	class VulkanContext;
	
	class VulkanPipeline : public IPipeline
	{
		friend class IPipeline;
	protected:
		VulkanPipeline(const std::string& name, const PipelineInputLayout& inputLayout, const std::string& vertexShader, const std::string& fragmentShader);
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
		
		VkShaderModule createShaderModule(const VulkanDevice& device, const std::vector<char>& code);
		VkVertexInputBindingDescription  getBindingDescription();
		std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
		void createPipeline();
	};
}
