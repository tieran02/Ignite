#include "igpch.h"
#include <Ignite/Log.h>
#include "platform/Vulkan/VulkanRendererAPI.h"
#include "platform/Vulkan/VulkanContext.h"

Ignite::VulkanRendererAPI::VulkanRendererAPI()
{
	Init();
}

void Ignite::VulkanRendererAPI::Init()
{
    LOG_CORE_INFO("Initialising VulkanRendererAPI");
}

void Ignite::VulkanRendererAPI::Cleanup()
{
	LOG_CORE_INFO("Cleaning VulkanRendererAPI");
}

Ignite::VulkanRendererAPI::~VulkanRendererAPI()
{
	Cleanup();
}

void Ignite::VulkanRendererAPI::BeginScene()
{
	//TODO start recording command buffers
	//free command buffers
	const VulkanContext* vulkanContext = reinterpret_cast<const VulkanContext*>(GetGraphicsContext());
	CORE_ASSERT(vulkanContext, "Failed to record scene, vulkan context is null");
	CORE_ASSERT(vulkanContext->CommandBuffers().size(), "Failed to record scene, vulkan command buffers are empty");

	vulkanContext->WaitTillFree();
	for (size_t i = 0; i < vulkanContext->CommandBuffers().size(); i++)
	{
		vkResetCommandBuffer(vulkanContext->CommandBuffers()[i], VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
	}

	//start recording
	for (size_t i = 0; i < vulkanContext->CommandBuffers().size(); i++)
	{
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0; // Optional
		beginInfo.pInheritanceInfo = nullptr; // Optional

		VK_CHECK_RESULT(vkBeginCommandBuffer(vulkanContext->CommandBuffers()[i], &beginInfo));

		//TODO renderpass should be seperate to pipeline
		//TODO start renderpass should start on begin scene
		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = vulkanContext->Renderpass().Renderpass();
		renderPassInfo.framebuffer = vulkanContext->Renderpass().FrameBuffers()[i];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = VkExtent2D{ vulkanContext->Swapchain().Width() ,vulkanContext->Swapchain().Height() };

		//clear colour as a test
		VkClearValue clearColor = { 0.2f, 0.2f, 0.2f, 1.0f };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		//begin render pass
		vkCmdBeginRenderPass(vulkanContext->CommandBuffers()[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);


		//LOG_CORE_INFO("Scene Recording Started");
	}
}	

void Ignite::VulkanRendererAPI::EndScene()
{
	const VulkanContext* vulkanContext = reinterpret_cast<const VulkanContext*>(GetGraphicsContext());
	CORE_ASSERT(vulkanContext, "Failed to record scene, vulkan context is null");
	CORE_ASSERT(vulkanContext->CommandBuffers().size(), "Failed to record scene, vulkan command buffers are empty");
	
	//TODO stop recording command buffers

	for (size_t i = 0; i < vulkanContext->CommandBuffers().size(); i++) 
	{
		//end renderpass
		vkCmdEndRenderPass(vulkanContext->CommandBuffers()[i]);

		//end recording
		VK_CHECK_RESULT(vkEndCommandBuffer(vulkanContext->CommandBuffers()[i]));
	}
	
	//LOG_CORE_INFO("Scene Recording Ended");
}

void Ignite::VulkanRendererAPI::SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {

}

void Ignite::VulkanRendererAPI::SetClearColor(const glm::vec4 &color) {

}

void Ignite::VulkanRendererAPI::Clear() {

}
