#pragma once
#include "vulkan/vulkan.h"

namespace Ignite
{
	class VulkanContext;

	class VulkenRenderpass
	{
	public:
		VulkenRenderpass(const VulkanContext& context);
		~VulkenRenderpass();

		const VkRenderPass& Renderpass() const { return m_renderpass; }
		const std::vector<VkFramebuffer>& FrameBuffers() const { return m_swapChainFramebuffers; }
	private:
		const VulkanContext& m_context;
		VkRenderPass m_renderpass;
		std::vector<VkFramebuffer> m_swapChainFramebuffers;

		VkImage depthImage;
		VkDeviceMemory depthImageMemory;
		VkImageView depthImageView;

		void create();
		void cleanup();
		void createFrameBuffers();

		VkFormat findDepthFormat() const;
		bool hasStencilComponent(VkFormat format);
		void createDepthResources();
	};
}


