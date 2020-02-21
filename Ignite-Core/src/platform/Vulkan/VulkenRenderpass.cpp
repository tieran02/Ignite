#include "igpch.h"
#include "platform/Vulkan/VulkenRenderpass.h"
#include "platform/Vulkan/VulkanContext.h"
#include "Ignite/Log.h"
#include "platform/Vulkan/VulkanTexture2D.h"
#include "platform/Vulkan/VulkanResources.h"

namespace Ignite
{
	VulkenRenderpass::VulkenRenderpass(const VulkanContext& context) : m_context(context)
	{
		create();
	}

	VulkenRenderpass::~VulkenRenderpass()
	{
		cleanup();
	}

	void VulkenRenderpass::create()
	{
		LOG_CORE_INFO("Creating vulkan renderpass");
		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = m_context.Swapchain().ImageFormat().format;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		//depth attachment
		VkAttachmentDescription depthAttachment = {};
		depthAttachment.format = findDepthFormat();
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		//subpasses/attacments
		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef = {};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;

		//Subpass dependencies
		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		VK_CHECK_RESULT(vkCreateRenderPass(m_context.Device().LogicalDevice(), &renderPassInfo, nullptr, &m_renderpass));

		createDepthResources();
		createFrameBuffers();
	}

	void VulkenRenderpass::cleanup()
	{
		vkDeviceWaitIdle(m_context.Device().LogicalDevice());

		vkDestroyImageView(m_context.Device().LogicalDevice(), depthImageView, nullptr);
		vkDestroyImage(m_context.Device().LogicalDevice(), depthImage, nullptr);
		vkFreeMemory(m_context.Device().LogicalDevice(), depthImageMemory, nullptr);
		
		LOG_CORE_INFO("Cleaning up vulkan framebuffers");
		for (auto framebuffer : m_swapChainFramebuffers) {
			vkDestroyFramebuffer(m_context.Device().LogicalDevice(), framebuffer, nullptr);
		}
		
		LOG_CORE_INFO("Cleaning up vulkan renderpass");
		vkDestroyRenderPass(m_context.Device().LogicalDevice(), m_renderpass, nullptr);
	}

	void VulkenRenderpass::createFrameBuffers()
	{
		LOG_CORE_INFO("Creating vulkan framebuffers");

		m_swapChainFramebuffers.resize(m_context.Swapchain().ImageViews().size());

		CORE_ASSERT(m_swapChainFramebuffers.size(), "Failed to create vulkan framebuffer as swapchain image view is empty")

			for (size_t i = 0; i < m_context.Swapchain().ImageViews().size(); i++) {
				std::array<VkImageView, 2> attachments = {
					m_context.Swapchain().ImageViews()[i],
					depthImageView
				};

				VkFramebufferCreateInfo framebufferInfo = {};
				framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
				framebufferInfo.renderPass = m_renderpass;
				framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
				framebufferInfo.pAttachments = attachments.data();
				framebufferInfo.width = m_context.Swapchain().Width();
				framebufferInfo.height = m_context.Swapchain().Height();
				framebufferInfo.layers = 1;

				VK_CHECK_RESULT(vkCreateFramebuffer(m_context.Device().LogicalDevice(), &framebufferInfo, nullptr, &m_swapChainFramebuffers[i]));
			}
	}

	VkFormat VulkenRenderpass::findDepthFormat() const
	{
		return m_context.FindSupportedFormat(
			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);
	}

	bool VulkenRenderpass::hasStencilComponent(VkFormat format)
	{
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
	}

	void VulkenRenderpass::createDepthResources()
	{
		VkFormat depthFormat = findDepthFormat();
		
		VulkanResources::CreateImage(m_context.Device().LogicalDevice(),m_context.Device().PhysicalDevice(),
			m_context.Swapchain().Width(), m_context.Swapchain().Height(), depthFormat,
			VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory);
		
		depthImageView = VulkanResources::CreateImageView(m_context.Device().LogicalDevice(), depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

		//VulkanTexture2D::TransitionImageLayout(m_context, depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

		
	}
}
