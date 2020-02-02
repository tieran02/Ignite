#include "igpch.h"
#include <Ignite/Log.h>
#include "platform/Vulkan/VulkanContext.h"
#include <Ignite/IWindow.h>
#include "Ignite/Application.h"
#include "Ignite/Renderer/IPipeline.h"

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
		m_renderpass = std::make_unique<VulkenRenderpass>(*this);

		createCommandPool();
		createCommandBuffers();
		createSyncObjects();
    }

	void VulkanContext::Cleanup()
	{
		vkDeviceWaitIdle(m_vulkanDevice->LogicalDevice());
		
		LOG_CORE_INFO("Cleaning up vulkan semaphores");
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			vkDestroySemaphore(m_vulkanDevice->LogicalDevice(), renderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(m_vulkanDevice->LogicalDevice(), imageAvailableSemaphores[i], nullptr);
			vkDestroyFence(m_vulkanDevice->LogicalDevice(), inFlightFences[i], nullptr);
		}
		
		LOG_CORE_INFO("Cleaning Vulkan Command pool");
		vkDestroyCommandPool(m_vulkanDevice->LogicalDevice(), m_commandPool, nullptr);

		//clean pipeline
		for (std::pair<std::string, std::shared_ptr<IPipeline>> pipeline : m_pipelines)
		{
			pipeline.second->Free();
		}
		
		m_renderpass.reset();
		m_vulkanSwapchain.reset();
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

		//clean pipeline
		for (std::pair<std::string, std::shared_ptr<IPipeline>> pipeline : m_pipelines)
		{
			pipeline.second->Free();
		}
		
		cleanupSwapchain();
		//TODO pass width as paramters along with viewport x,y
		m_vulkanSwapchain = std::make_unique<VulkanSwapChain>(*m_vulkanDevice, width, height);
		m_renderpass = std::make_unique<VulkenRenderpass>(*this);
		
		//clean pipeline
		for (std::pair<std::string, std::shared_ptr<IPipeline>> pipeline : m_pipelines)
		{
			pipeline.second->Recreate();
		}

		createCommandBuffers();
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
		//free command buffers
		vkFreeCommandBuffers(m_vulkanDevice->LogicalDevice(), m_commandPool, static_cast<uint32_t>(m_commandBuffers.size()), m_commandBuffers.data());
		m_renderpass.reset();
		m_vulkanSwapchain.reset();
	}
}
