#include "igpch.h"
#include "platform/Vulkan/VulkenRenderpass.h"
#include "platform/Vulkan/VulkanContext.h"
#include "Ignite/Log.h"

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

		//subpasses/attacments
		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		//create renderpass
		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;

		//Subpass dependencies
		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		VK_CHECK_RESULT(vkCreateRenderPass(m_context.Device().LogicalDevice(), &renderPassInfo, nullptr, &m_renderpass));

		createFrameBuffers();
	}

	void VulkenRenderpass::cleanup()
	{
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
				VkImageView attachments[] = {
					m_context.Swapchain().ImageViews()[i]
				};

				VkFramebufferCreateInfo framebufferInfo = {};
				framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
				framebufferInfo.renderPass = m_renderpass;
				framebufferInfo.attachmentCount = 1;
				framebufferInfo.pAttachments = attachments;
				framebufferInfo.width = m_context.Swapchain().Width();
				framebufferInfo.height = m_context.Swapchain().Height();
				framebufferInfo.layers = 1;

				VK_CHECK_RESULT(vkCreateFramebuffer(m_context.Device().LogicalDevice(), &framebufferInfo, nullptr, &m_swapChainFramebuffers[i]));
			}
	}
}
