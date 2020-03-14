#include "igpch.h"
#include <Ignite/Log.h>
#include "platform/Vulkan/VulkanRendererAPI.h"
#include "platform/Vulkan/VulkanContext.h"
#include "platform/Vulkan/VulkanPipeline.h"

Ignite::VulkanRendererAPI::VulkanRendererAPI()
{
	Init();
}

void Ignite::VulkanRendererAPI::Init()
{
    LOG_CORE_INFO("Initialising VulkanRendererAPI");
	createDescriptorSets();
}

void Ignite::VulkanRendererAPI::Cleanup()
{
	LOG_CORE_INFO("Cleaning VulkanRendererAPI");
}

Ignite::VulkanRendererAPI::~VulkanRendererAPI()
{
	Cleanup();
}

void Ignite::VulkanRendererAPI::BeginScene(const Camera& camera)
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

		vkCmdBindDescriptorSets(vulkanContext->CommandBuffers()[i], VK_PIPELINE_BIND_POINT_GRAPHICS, VulkanPipeline::PipelineLayout(), 0, 1,
			&m_sceneDescriptorSets[i], 0, nullptr);
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

void Ignite::VulkanRendererAPI::DrawIndexed(const IVertexBuffer* vertexBuffer, const IIndexBuffer* indexBuffer, uint32_t indexCount)
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

void Ignite::VulkanRendererAPI::SetSceneUniformBufferObject(const SceneUniformBuffer& ubo)
{
	const VulkanContext* vulkanContext = reinterpret_cast<VulkanContext*>(GetGraphicsContext());
	CORE_ASSERT(vulkanContext, "Failed to set uniform buffer object, vulkan context is null");
	CORE_ASSERT(!vulkanContext->SceneUniformBuffers().empty(), "Failed to set uniform buffer object, no uniform buffers");

	for (size_t i = 0; i < vulkanContext->SceneUniformBuffers().size(); i++)
	{
		void* data;
		vkMapMemory(vulkanContext->Device().LogicalDevice(), vulkanContext->SceneUniformBuffers()[i]->DeviceMemory(), 0, sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(vulkanContext->Device().LogicalDevice(), vulkanContext->SceneUniformBuffers()[i]->DeviceMemory());
	}

	//TODO temp
	ModelUniformBuffer modelUBO;
	for (size_t i = 0; i < vulkanContext->ModelUniformBuffers().size(); i++)
	{
		void* data;
		vkMapMemory(vulkanContext->Device().LogicalDevice(), vulkanContext->ModelUniformBuffers()[i]->DeviceMemory(), 0, sizeof(modelUBO), 0, &data);
		memcpy(data, &modelUBO, sizeof(modelUBO));
		vkUnmapMemory(vulkanContext->Device().LogicalDevice(), vulkanContext->ModelUniformBuffers()[i]->DeviceMemory());
	}
}

void Ignite::VulkanRendererAPI::SetClearColor(const glm::vec4 &color)
{
	m_clearColour = color;
}

void Ignite::VulkanRendererAPI::Clear()
{

}

void Ignite::VulkanRendererAPI::createDescriptorSets()
{
	const VulkanContext* vulkanContext = reinterpret_cast<const VulkanContext*>(m_graphicsContext.get());
	CORE_ASSERT(vulkanContext, "Failed to create descriptorsets, vulkan context is null");

	std::vector<VkDescriptorSetLayout> layouts(vulkanContext->Swapchain().ImageViews().size(), vulkanContext->ModelDescriptorSetLayout());

	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = vulkanContext->DescriptorPool();
	allocInfo.descriptorSetCount = static_cast<uint32_t>(vulkanContext->Swapchain().ImageViews().size());
	allocInfo.pSetLayouts = layouts.data();

	m_sceneDescriptorSets.resize(vulkanContext->Swapchain().ImageViews().size());
	VkResult r = vkAllocateDescriptorSets(vulkanContext->Device().LogicalDevice(), &allocInfo, m_sceneDescriptorSets.data());

	for (size_t i = 0; i < vulkanContext->Swapchain().ImageViews().size(); i++)
	{
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = vulkanContext->SceneUniformBuffers()[i]->Buffer();
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(SceneUniformBuffer);

		VkWriteDescriptorSet descriptorWrite = {};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = m_sceneDescriptorSets[i];
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;

		std::array<VkWriteDescriptorSet, 1> write_descriptors{ descriptorWrite };
		vkUpdateDescriptorSets(vulkanContext->Device().LogicalDevice(), write_descriptors.size(), write_descriptors.data(), 0, nullptr);
	}
}

void Ignite::VulkanRendererAPI::bindDescriptors()
{
	const VulkanContext* vulkanContext = reinterpret_cast<const VulkanContext*>(m_graphicsContext.get());
	CORE_ASSERT(vulkanContext, "Failed to bind Descriptors, vulkan context is null");

	VkPipelineLayout layout = VulkanPipeline::PipelineLayout();

	for (size_t i = 0; i < vulkanContext->CommandBuffers().size(); i++)
	{
		vkCmdBindDescriptorSets(vulkanContext->CommandBuffers()[i], VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 1, 1,
			&m_sceneDescriptorSets[i], 0, nullptr);
	}
}
