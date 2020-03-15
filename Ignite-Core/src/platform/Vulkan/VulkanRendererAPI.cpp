#include "igpch.h"
#include <Ignite/Log.h>
#include "platform/Vulkan/VulkanRendererAPI.h"
#include "platform/Vulkan/VulkanContext.h"
#include "platform/Vulkan/VulkanPipeline.h"
#include "Ignite/Renderer/Camera.h"
#include "Ignite/Renderer/Renderer.h"

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

void Ignite::VulkanRendererAPI::BeginScene(const Camera& camera, const std::vector<Light>& lights)
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
		std::array<VkClearValue, 2> clearValues = {};
		clearValues[0].color = { m_clearColour.r, m_clearColour.g, m_clearColour.b, m_clearColour.a };;
		clearValues[1].depthStencil = { 1.0f, 0 };
		
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		//begin render pass
		vkCmdBeginRenderPass(vulkanContext->CommandBuffers()[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		//LOG_CORE_INFO("Scene Recording Started");

		//upload scene ubo (Camera/View)
		vkCmdBindDescriptorSets(vulkanContext->CommandBuffers()[i], VK_PIPELINE_BIND_POINT_GRAPHICS, VulkanPipeline::PipelineLayout(), 0, 1,
			&vulkanContext->SceneDescriptorSets()[i], 0, nullptr);

		void* sceneData;
		vkMapMemory(vulkanContext->Device().LogicalDevice(), vulkanContext->SceneUniformBuffers()[i]->DeviceMemory(), 0, sizeof(Renderer::SceneUBO()), 0, &sceneData);
		memcpy(sceneData, &Renderer::SceneUBO(), sizeof(Renderer::SceneUBO()));
		vkUnmapMemory(vulkanContext->Device().LogicalDevice(), vulkanContext->SceneUniformBuffers()[i]->DeviceMemory());
		
		//upload lights
		vkCmdBindDescriptorSets(vulkanContext->CommandBuffers()[i], VK_PIPELINE_BIND_POINT_GRAPHICS, VulkanPipeline::PipelineLayout(), 2, 1,
			&vulkanContext->LightDescriptorSets()[i], 0, nullptr);

		LightBuffer lightBuffer{ lights.size(), lights[0] };
		
		void* lightData;
		vkMapMemory(vulkanContext->Device().LogicalDevice(), vulkanContext->LightStorageBuffers()[i]->DeviceMemory(), 0, sizeof(lightBuffer), 0, &lightData);
		memcpy(lightData, &lightBuffer, sizeof(lightBuffer));

		vkUnmapMemory(vulkanContext->Device().LogicalDevice(), vulkanContext->LightStorageBuffers()[i]->DeviceMemory());

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

void Ignite::VulkanRendererAPI::DrawIndexed(const IVertexBuffer* vertexBuffer, const IIndexBuffer* indexBuffer, uint32_t indexCount, const glm::mat4& transform)
{
	//TODO when binding a pipeline we need to create a new command to start the render pass
	const VulkanContext* vulkanContext = reinterpret_cast<VulkanContext*>(GetGraphicsContext());
	CORE_ASSERT(vulkanContext, "Failed to draw index, vulkan context is null");
	CORE_ASSERT(vulkanContext->CommandBuffers().size(), "Failed to draw index, vulkan command buffers are empty");
	
	for (size_t i = 0; i < vulkanContext->CommandBuffers().size(); i++)
	{
		ModelUniformBuffer ubo{ transform };
		vkCmdPushConstants(
			vulkanContext->CommandBuffers()[i],
			VulkanPipeline::PipelineLayout(),
			VK_SHADER_STAGE_VERTEX_BIT,
			0,
			sizeof(ModelUniformBuffer),
			&ubo);
		
		//draw test
		//TODO add index vertexBuffer
		vkCmdDrawIndexed(vulkanContext->CommandBuffers()[i], static_cast<uint32_t>(indexCount), 1, 0, 0, 0);
	}
	
}

void Ignite::VulkanRendererAPI::SetModelUniformBufferObject(const ModelUniformBuffer& ubo)
{

}

void Ignite::VulkanRendererAPI::SetClearColor(const glm::vec4 &color)
{
	m_clearColour = color;
}

void Ignite::VulkanRendererAPI::Clear()
{

}

void Ignite::VulkanRendererAPI::bindDescriptors()
{
	const VulkanContext* vulkanContext = reinterpret_cast<const VulkanContext*>(m_graphicsContext.get());
	CORE_ASSERT(vulkanContext, "Failed to bind Descriptors, vulkan context is null");

	VkPipelineLayout layout = VulkanPipeline::PipelineLayout();

	for (size_t i = 0; i < vulkanContext->CommandBuffers().size(); i++)
	{
		vkCmdBindDescriptorSets(vulkanContext->CommandBuffers()[i], VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 1, 1,
			&vulkanContext->SceneDescriptorSets()[i], 0, nullptr);
	}
}
