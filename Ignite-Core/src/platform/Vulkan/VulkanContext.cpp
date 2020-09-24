#include "igpch.h"
#include <Ignite/Log.h>
#include "platform/Vulkan/VulkanContext.h"
#include <Ignite/IWindow.h>
#include "Ignite/Application.h"
#include "Ignite/Renderer/IPipeline.h"
#include "Ignite/Renderer/ITexture2D.h"
#include "platform/Vulkan/VulkanTexture2D.h"
#include "platform/Vulkan/VulkanMesh.h"
#include "platform/Vulkan/VulkanMaterial.h"
#include "Ignite/Renderer/Renderer.h"

namespace Ignite
{
	VulkanContext::VulkanContext() :IGraphicsContext()
    {
        Init();
    }

    VulkanContext::~VulkanContext()
    {
		Cleanup();
    }

    void VulkanContext::Init()
    {
		LOG_CORE_INFO("Initialising VulkanContext");
		m_vulkanDevice = std::make_unique<VulkanDevice>();
		m_vulkanSwapchain = std::make_unique<VulkanSwapChain>(*m_vulkanDevice, Application::Instance().Window()->Width(), Application::Instance().Window()->Height());
		createDescriptorSetLayout();
		
		createUniformBuffers();
		createDescriptorPool();
		createSceneDescriptorSets();
		createCommandPool();
		createCommandBuffers();
		createSyncObjects();

		m_renderpass = std::make_unique<VulkenRenderpass>(*this);
    }

	void VulkanContext::Cleanup()
	{
		vkDeviceWaitIdle(m_vulkanDevice->LogicalDevice());

		//clean images
		for (auto& image : m_texture2Ds)
		{
			image.second->Free();
		}
		
		//clean buffers
		for (auto& buffer : m_buffers)
		{
			buffer.second->Free();
		}
		
		cleanupSwapchain();

		vkDestroyDescriptorSetLayout(m_vulkanDevice->LogicalDevice(), m_sceneDescriptorSetLayout, nullptr);
		vkDestroyDescriptorSetLayout(m_vulkanDevice->LogicalDevice(), m_materialDescriptorSetLayout, nullptr);
		vkDestroyDescriptorSetLayout(m_vulkanDevice->LogicalDevice(), m_lightDescriptorSetLayout, nullptr);

		if(VulkanPipeline::PipelineLayout() != VK_NULL_HANDLE)
			vkDestroyPipelineLayout(m_vulkanDevice->LogicalDevice(), VulkanPipeline::PipelineLayout(), nullptr);
		
		LOG_CORE_INFO("Cleaning up vulkan semaphores");
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			vkDestroySemaphore(m_vulkanDevice->LogicalDevice(), renderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(m_vulkanDevice->LogicalDevice(), imageAvailableSemaphores[i], nullptr);
			vkDestroyFence(m_vulkanDevice->LogicalDevice(), inFlightFences[i], nullptr);
		}
		
		LOG_CORE_INFO("Cleaning Vulkan Command pool");
		vkDestroyCommandPool(m_vulkanDevice->LogicalDevice(), m_commandPool, nullptr);
		
		m_vulkanDevice.reset();
		LOG_CORE_INFO("Cleaning VulkanContext");
	}

	void VulkanContext::SwapBuffers()
    {
		//wait for in flight fences
		vkWaitForFences(m_vulkanDevice->LogicalDevice(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

		//draw frame
		uint32_t imageIndex;

		//TODO recrate swapchain if not compatible 
		vkAcquireNextImageKHR(m_vulkanDevice->LogicalDevice(), m_vulkanSwapchain->Swapchain(), UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

		//TODO update ubo
		
		
		// Check if a previous frame is using this image (i.e. there is its fence to wait on)
		if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
			vkWaitForFences(m_vulkanDevice->LogicalDevice(), 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
		}
		// Mark the image as now being in use by this frame
		imagesInFlight[imageIndex] = inFlightFences[currentFrame];

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_commandBuffers[imageIndex];

		VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		
		//reset command buffer fence
		//vkResetFences(m_vulkanDevice->LogicalDevice(), 1, &m_commnadBufferFence);
		vkResetFences(m_vulkanDevice->LogicalDevice(), 1, &inFlightFences[currentFrame]);
		
		VK_CHECK_RESULT(vkQueueSubmit(m_vulkanDevice->GraphicsQueue(), 1, &submitInfo, inFlightFences[currentFrame]));

		//Presentation
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { m_vulkanSwapchain->Swapchain() };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr; // Optional
		
		vkQueuePresentKHR(m_vulkanDevice->PresentQueue(), &presentInfo);
		
		currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

    }

	void VulkanContext::WaitTillFree() const
	{
		vkWaitForFences(m_vulkanDevice->LogicalDevice(), MAX_FRAMES_IN_FLIGHT, &inFlightFences[0], VK_TRUE, UINT64_MAX);
	}

	void VulkanContext::RecreateSwapchain(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		vkDeviceWaitIdle(m_vulkanDevice->LogicalDevice());
		
		cleanupSwapchain();
		//TODO pass width as paramters along with viewport x,y
		m_vulkanSwapchain = std::make_unique<VulkanSwapChain>(*m_vulkanDevice, width, height);
		m_renderpass = std::make_unique<VulkenRenderpass>(*this);
		
		//clean pipeline
		for (auto& pipeline : m_pipelines)
		{
			pipeline.second->Recreate();
		}

		createUniformBuffers();
		createDescriptorPool();

		//create scene descriptor sets
		createSceneDescriptorSets();
		
		//recreate model descriptor sets
		for (const auto& mesh : m_meshes)
		{
			VulkanMesh* vulkanModel = static_cast<VulkanMesh*>(mesh.second.get());
			vulkanModel->CreateDescriptorSet();
		}

		for (auto& material : m_materials)
		{
			VulkanMaterial* vulkanMaterial = static_cast<VulkanMaterial*>(material.second.get());
			vulkanMaterial->CreateDescriptorSets();
		}
		
		createCommandBuffers();
	}

	VkFormat VulkanContext::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling,
		VkFormatFeatureFlags features) const
	{
		for(VkFormat format : candidates) {
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(m_vulkanDevice->PhysicalDevice(), format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
				return format;
			}
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
				return format;
			}
		}

		LOG_CORE_FATAL("failed to find supported format!");
		return VK_FORMAT_UNDEFINED;
	}

	void VulkanContext::createUniformBuffers()
	{
		m_sceneUniformBuffers.resize(m_vulkanSwapchain->ImageViews().size());
		m_lightStorageBuffers.resize(m_vulkanSwapchain->ImageViews().size());
		
		//create a ubo for each swapcahin image
		for (size_t i = 0; i < m_vulkanSwapchain->ImageViews().size(); i++) 
		{
			//Create scene unform buffers (Camera projection/view and lights)
			m_sceneUniformBuffers[i] = std::unique_ptr<VulkanBaseBuffer>(new VulkanBaseBuffer(this));
			SceneUniformBuffer sceneUBO{};
			m_sceneUniformBuffers[i]->Create(&sceneUBO, sizeof(SceneUniformBuffer), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, BUFFER_VISIBILITY::HOST);

			//create light storage buffers
			size_t lightBufferSize = sizeof(LightBuffer);
			m_lightStorageBuffers[i] = std::unique_ptr<VulkanBaseBuffer>(new VulkanBaseBuffer(this));
			m_lightStorageBuffers[i]->Create(nullptr, lightBufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, BUFFER_VISIBILITY::HOST);
			
		}
	}

	void VulkanContext::createDescriptorPool()
	{
		LOG_CORE_INFO("Creating Descriptor Pool");
		std::array<VkDescriptorPoolSize, 3> poolSizes = {};
		//scene ubo set
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = static_cast<uint32_t>(m_vulkanSwapchain->ImageViews().size());
		//texture set
		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[1].descriptorCount = static_cast<uint32_t>(m_vulkanSwapchain->ImageViews().size());
		//light storage buffer
		poolSizes[2].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		poolSizes[2].descriptorCount = static_cast<uint32_t>(m_vulkanSwapchain->ImageViews().size());

		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = 2000;

		VK_CHECK_RESULT(vkCreateDescriptorPool(m_vulkanDevice->LogicalDevice(), &poolInfo, nullptr, &descriptorPool));
	}

	void VulkanContext::createDescriptorSetLayout()
	{
		// Descriptor set and pipeline layouts
		std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings;
		
		//scene descriptors
		VkDescriptorSetLayoutBinding sceneUBOLayoutBinding = {};
		sceneUBOLayoutBinding.binding = 0;
		sceneUBOLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		sceneUBOLayoutBinding.descriptorCount = 1;
		sceneUBOLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		sceneUBOLayoutBinding.pImmutableSamplers = nullptr; // Optional
		setLayoutBindings.push_back(sceneUBOLayoutBinding);

		VkDescriptorSetLayoutCreateInfo sceneLayoutInfo = {};
		sceneLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		sceneLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
		sceneLayoutInfo.pBindings = setLayoutBindings.data();
		
		VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_vulkanDevice->LogicalDevice(), &sceneLayoutInfo, nullptr, &m_sceneDescriptorSetLayout));
		
		setLayoutBindings.clear();
		//material descriptors
		VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
		samplerLayoutBinding.binding = 0;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.pImmutableSamplers = nullptr;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		setLayoutBindings.push_back(samplerLayoutBinding);
		//specular
		VkDescriptorSetLayoutBinding specularLayoutBinding = {};
		specularLayoutBinding.binding = 1;
		specularLayoutBinding.descriptorCount = 1;
		specularLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		specularLayoutBinding.pImmutableSamplers = nullptr;
		specularLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		setLayoutBindings.push_back(specularLayoutBinding);

		//normal mask
		VkDescriptorSetLayoutBinding normalLayoutBinding = {};
		normalLayoutBinding.binding = 2;
		normalLayoutBinding.descriptorCount = 1;
		normalLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		normalLayoutBinding.pImmutableSamplers = nullptr;
		normalLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		setLayoutBindings.push_back(normalLayoutBinding);
		
		//alpha mask
		VkDescriptorSetLayoutBinding alphaLayoutBinding = {};
		alphaLayoutBinding.binding = 3;
		alphaLayoutBinding.descriptorCount = 1;
		alphaLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		alphaLayoutBinding.pImmutableSamplers = nullptr;
		alphaLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		setLayoutBindings.push_back(alphaLayoutBinding);

		VkDescriptorSetLayoutCreateInfo materialLayoutInfo = {};
		materialLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		materialLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
		materialLayoutInfo.pBindings = setLayoutBindings.data();

		VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_vulkanDevice->LogicalDevice(), &materialLayoutInfo, nullptr, &m_materialDescriptorSetLayout));

		//light descriptor sets
		setLayoutBindings.clear();
		
		//light descriptors
		VkDescriptorSetLayoutBinding lightUBOLayoutBinding = {};
		lightUBOLayoutBinding.binding = 0;
		lightUBOLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		lightUBOLayoutBinding.descriptorCount = 1;
		lightUBOLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		lightUBOLayoutBinding.pImmutableSamplers = nullptr; // Optional
		setLayoutBindings.push_back(lightUBOLayoutBinding);

		VkDescriptorSetLayoutCreateInfo lightLayoutInfo = {};
		lightLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		lightLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
		lightLayoutInfo.pBindings = setLayoutBindings.data();

		VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_vulkanDevice->LogicalDevice(), &lightLayoutInfo, nullptr, &m_lightDescriptorSetLayout));
	}

	void VulkanContext::createSceneDescriptorSets()
	{
		std::vector<VkDescriptorSetLayout> layouts(Swapchain().ImageViews().size(), SceneDescriptorSetLayout());

		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = DescriptorPool();
		allocInfo.descriptorSetCount = static_cast<uint32_t>(Swapchain().ImageViews().size());
		allocInfo.pSetLayouts = layouts.data();

		m_sceneDescriptorSets.resize(Swapchain().ImageViews().size());
		VkResult r = vkAllocateDescriptorSets(Device().LogicalDevice(), &allocInfo, m_sceneDescriptorSets.data());

		for (size_t i = 0; i < Swapchain().ImageViews().size(); i++)
		{
			VkDescriptorBufferInfo bufferInfo = {};
			bufferInfo.buffer = SceneUniformBuffers()[i]->Buffer();
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
			vkUpdateDescriptorSets(Device().LogicalDevice(), static_cast<uint32_t>(write_descriptors.size()), write_descriptors.data(), 0, nullptr);
		}

		//light storage buffer
		layouts = std::vector<VkDescriptorSetLayout>(Swapchain().ImageViews().size(), LightDescriptorSetLayout());

		allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = DescriptorPool();
		allocInfo.descriptorSetCount = static_cast<uint32_t>(Swapchain().ImageViews().size());
		allocInfo.pSetLayouts = layouts.data();

		m_lightDescriptorSets.resize(Swapchain().ImageViews().size());
		r = vkAllocateDescriptorSets(Device().LogicalDevice(), &allocInfo, m_lightDescriptorSets.data());

		size_t lightBufferSize = sizeof(LightBuffer);
		for (size_t i = 0; i < Swapchain().ImageViews().size(); i++)
		{
			VkDescriptorBufferInfo bufferInfo = {};
			bufferInfo.buffer = m_lightStorageBuffers[i]->Buffer();
			bufferInfo.offset = 0;
			bufferInfo.range = lightBufferSize;

			VkWriteDescriptorSet descriptorWrite = {};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = m_lightDescriptorSets[i];
			descriptorWrite.dstBinding = 0;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &bufferInfo;

			std::array<VkWriteDescriptorSet, 1> write_descriptors{ descriptorWrite };
			vkUpdateDescriptorSets(Device().LogicalDevice(), static_cast<uint32_t>(write_descriptors.size()), write_descriptors.data(), 0, nullptr);
		}
	}

	void VulkanContext::createCommandPool()
	{
		LOG_CORE_INFO("Creating Vulkan Command Pool");
		CORE_ASSERT(m_vulkanDevice, "Failed to create command pool vulkan device is null");
		
		QueueFamilyIndices queueFamilyIndices = m_vulkanDevice->QueueFamilies();

		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Optional
		
		VK_CHECK_RESULT(vkCreateCommandPool(m_vulkanDevice->LogicalDevice(), &poolInfo, nullptr, &m_commandPool));
	}

	void VulkanContext::createCommandBuffers()
	{
		LOG_CORE_INFO("Creating Vulkan Command Buffers");
		CORE_ASSERT(m_vulkanDevice, "Failed to create command buffers, vulkan device is null");
		CORE_ASSERT(m_vulkanSwapchain, "Failed to create command buffers, swapchain is null");
		CORE_ASSERT(m_vulkanSwapchain->ImageViews().size(), "Failed to create command buffers, swapchain imagevies is empty");
		
		m_commandBuffers.resize(m_vulkanSwapchain->ImageViews().size());
		
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = m_commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = (uint32_t)m_commandBuffers.size();

		VK_CHECK_RESULT(vkAllocateCommandBuffers(m_vulkanDevice->LogicalDevice(), &allocInfo, m_commandBuffers.data()));
	}

	void VulkanContext::createSyncObjects()
	{
		LOG_CORE_INFO("Creating vulkan semaphores");
		CORE_ASSERT(m_vulkanDevice, "Failed to create semaphores, vulkan device is null");
		
		imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
		imagesInFlight.resize(m_vulkanSwapchain->Images().size(), VK_NULL_HANDLE);

		
		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo = {};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
		{
			VK_CHECK_RESULT(vkCreateSemaphore(m_vulkanDevice->LogicalDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]));
			VK_CHECK_RESULT(vkCreateSemaphore(m_vulkanDevice->LogicalDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]));
			VK_CHECK_RESULT(vkCreateFence(m_vulkanDevice->LogicalDevice(), &fenceInfo, nullptr, &inFlightFences[i]));
		}
	}

	void VulkanContext::cleanupSwapchain()
	{

		LOG_CORE_INFO("Cleaning up vulkan swapchain for recreation");
		
		//clean pipeline
		for (auto& pipeline : m_pipelines)
		{
			pipeline.second->Free();
		}
		
		//free command buffers
		vkFreeCommandBuffers(m_vulkanDevice->LogicalDevice(), m_commandPool, static_cast<uint32_t>(m_commandBuffers.size()), m_commandBuffers.data());
		m_renderpass.reset();
		m_vulkanSwapchain.reset();

		LOG_CORE_INFO("Cleaning up vulkan uniform buffers");
		for (size_t i = 0; i < m_sceneUniformBuffers.size(); i++)
		{
			m_sceneUniformBuffers[i]->Free();
			m_lightStorageBuffers[i]->Free();
		}
		
		LOG_CORE_INFO("Cleaning up vulkan Descriptor Pool");
		vkDestroyDescriptorPool(m_vulkanDevice->LogicalDevice(), descriptorPool, nullptr);
	}
}
