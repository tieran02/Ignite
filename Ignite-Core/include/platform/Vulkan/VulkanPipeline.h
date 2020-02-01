#pragma once
#include "Ignite/IPipeline.h"
#include <vulkan/vulkan_core.h>

namespace  Ignite
{
	class VulkanDevice;
	class VulkanContext;
	
	class VulkanPipeline : public IPipeline
	{
		friend class IPipeline;
	protected:
		VulkanPipeline(const std::string& vertexShader, const std::string& fragmentShader);
	public:
		~VulkanPipeline() override;
	protected:
		void Init() override;
		void Cleanup() override;
		
	public:
		void Bind() const override;
		void Unbind() const override;	
	private:
		VkPipelineLayout m_pipelineLayout;
		VkRenderPass m_renderPass;
		VkPipeline m_pipeline;

		std::vector<VkFramebuffer> m_swapChainFramebuffers;
		
		VkShaderModule createShaderModule(const VulkanDevice& device, const std::vector<char>& code);
		void createPipeline();
		void createRenderPass();

		//todo move framebuffer out of pipeline
		void createFrameBuffers();
	};
}
