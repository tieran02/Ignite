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
		VulkanPipeline(const std::string& name, const std::string& vertexShader, const std::string& fragmentShader);
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
	private:
		VkPipelineLayout m_pipelineLayout;
		VkPipeline m_pipeline;
		bool m_deleted;
		
		VkShaderModule createShaderModule(const VulkanDevice& device, const std::vector<char>& code);
		void createPipeline();
	};
}
