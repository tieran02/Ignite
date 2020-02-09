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
		VkClearValue clearColor = { m_clearColour.r, m_clearColour.g, m_clearColour.b, m_clearColour.a };
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

void Ignite::VulkanRendererAPI::SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	VulkanContext* vulkanContext = reinterpret_cast<VulkanContext*>(GetGraphicsContext());
	vulkanContext->RecreateSwapchain(x,y,width,height);
}

void Ignite::VulkanRendererAPI::DrawIndexed(const IVertexBuffer* vertexBuffer, const IIndexBuffer* indexBuffer, uint16_t indexCount)
{
	//TODO when binding a pipeline we need to create a new command to start the render pass
	const VulkanContext* vulkanContext = reinterpret_cast<VulkanContext*>(GetGraphicsContext());
	CORE_ASSERT(vulkanContext, "Failed to draw index, vulkan context is null");
	CORE_ASSERT(vulkanContext->CommandBuffers().size(), "Failed to draw index, vulkan command buffers are empty");

	for (size_t i = 0; i < vulkanContext->CommandBuffers().size(); i++) {

		//draw test
		//TODO add index vertexBuffer
		vkCmdDrawIndexed(vulkanContext->CommandBuffers()[i], static_cast<uint32_t>(indexCount), 1, 0, 0, 0);
	}
	
}

void Ignite::VulkanRendererAPI::SetUniformBufferObject(const UniformBufferObject& ubo)
{
	const VulkanContext* vulkanContext = reinterpret_cast<VulkanContext*>(GetGraphicsContext());
	CORE_ASSERT(vulkanContext, "Failed to set uniform buffer object, vulkan context is null");
	CORE_ASSERT(!vulkanContext->UniformBuffers().empty(), "Failed to set uniform buffer object, no uniform buffers");

	for (size_t i = 0; i < vulkanContext->UniformBuffers().size(); i++)
	{
		void* data;
		vkMapMemory(vulkanContext->Device().LogicalDevice(), vulkanContext->UniformBuffers()[i]->DeviceMemory(), 0, sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(vulkanContext->Device().LogicalDevice(), vulkanContext->UniformBuffers()[i]->DeviceMemory());
	}
}

void Ignite::VulkanRendererAPI::SetClearColor(const glm::vec4 &color)
{
	m_clearColour = color;
}

void Ignite::VulkanRendererAPI::Clear()
{

}
