#pragma once

#include <Ignite/Renderer/IGraphicsContext.h>
#include <platform\Vulkan\VulkanDevice.h>
#include "VulkanSwapChain.h"
#include "VulkenRenderpass.h"

#define VK_CHECK_RESULT(f)																				\
{																										\
	VkResult res = (f);																					\
		CORE_ASSERT(res == VK_SUCCESS, "Fatal : VkResult is " + std::to_string(static_cast<int>(res)) + " in " + __FILE__ + " at line " + std::to_string(__LINE__));\
}

namespace Ignite
{
    class VulkanContext : public IGraphicsContext
    {
	friend class IGraphicsContext;
    protected:
        VulkanContext();

		void Init() override;
		void Cleanup() override;
	public:
		const VulkanDevice& Device() const { return *m_vulkanDevice; }
		const VulkanSwapChain& Swapchain() const { return *m_vulkanSwapchain; }
		const VulkenRenderpass& Renderpass() const { return *m_renderpass; }
		const VkCommandPool& CommandPool() const { return  m_commandPool; }
		const std::vector<VkCommandBuffer>& CommandBuffers() const { return m_commandBuffers; }

		~VulkanContext();

		void SwapBuffers() override;
		void WaitTillFree() const;
		void RecreateSwapchain(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
	private:
		const int MAX_FRAMES_IN_FLIGHT = 2;

		std::unique_ptr<VulkanDevice> m_vulkanDevice;
		std::unique_ptr<VulkanSwapChain> m_vulkanSwapchain;
		std::unique_ptr<VulkenRenderpass> m_renderpass;

    	//comands
		VkCommandPool m_commandPool;
		std::vector<VkCommandBuffer> m_commandBuffers;

    	//semaphores
		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;
		std::vector<VkFence> imagesInFlight;
		size_t currentFrame = 0;

		void createCommandPool();
		void createCommandBuffers();
		void createSyncObjects();
		void cleanupSwapchain();
    };
}