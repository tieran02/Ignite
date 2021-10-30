#pragma once

#include <Ignite/Renderer/GraphicsContext.h>
#include <platform\Vulkan\VulkanDevice.h>
#include "VulkanSwapChain.h"
#include "VulkenRenderpass.h"
#include "VulkanBuffer.h"

#define VK_CHECK_RESULT(f)																				\
{																										\
	VkResult res = (f);																					\
		CORE_ASSERT(res == VK_SUCCESS, "Fatal : VkResult is " + std::to_string(static_cast<int>(res)) + " in " + __FILE__ + " at line " + std::to_string(__LINE__));\
}

namespace Ignite
{
    class VulkanContext : public GraphicsContext
    {
	friend class GraphicsContext;
    protected:
        VulkanContext();

		void Init() override;
		void Cleanup() override;
	public:
		VulkanDevice& Device() { return *m_vulkanDevice; }
		const VulkanDevice& Device() const { return *m_vulkanDevice; }
		const VulkanSwapChain& Swapchain() const { return *m_vulkanSwapchain; }
		const VulkenRenderpass& Renderpass() const { return *m_renderpass; }
		const VkCommandPool& CommandPool() const { return  m_commandPool; }
		const std::vector<VkCommandBuffer>& CommandBuffers() const { return m_commandBuffers; }
		const std::vector<VulkanBuffer>& SceneUniformBuffers() const { return m_sceneUniformBuffers; }
		const std::vector<VulkanBuffer>& LightStorageBuffers() const { return m_lightStorageBuffers; }
		const VkDescriptorPool& DescriptorPool() const { return descriptorPool; }
		const VkDescriptorSetLayout& SceneDescriptorSetLayout() const { return m_sceneDescriptorSetLayout; }
		const VkDescriptorSetLayout& MaterialDescriptorSetLayout() const { return m_materialDescriptorSetLayout; }
		const VkDescriptorSetLayout& LightDescriptorSetLayout() const { return m_lightDescriptorSetLayout; }
		const std::vector<VkDescriptorSet>& SceneDescriptorSets() const { return m_sceneDescriptorSets; }
		const std::vector<VkDescriptorSet>& LightDescriptorSets() const { return m_lightDescriptorSets; }
		const VkFence& InFlightFence() const { return inFlightFences[currentFrame]; }
		const VkFence& ImageInFlightFence() const { return imagesInFlight[currentFrame]; }
		~VulkanContext();

		void SwapBuffers() override;
		void CreateDescriptorSetLayouts() override;
		void WaitTillFree() const;
		void RecreateSwapchain(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

		VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;

	private:
		const int MAX_FRAMES_IN_FLIGHT = 2;

		std::unique_ptr<VulkanDevice> m_vulkanDevice;
		std::unique_ptr<VulkanSwapChain> m_vulkanSwapchain;
		std::unique_ptr<VulkenRenderpass> m_renderpass;

    	//uniform buffers
		std::vector<VulkanBuffer> m_sceneUniformBuffers;
		std::vector<VulkanBuffer> m_lightStorageBuffers;
    	
    	//descriptor pool/sets
		VkDescriptorPool descriptorPool;
		std::unordered_map<UUID, VkDescriptorSetLayout> m_vkDescriptorSetLayouts;
		VkDescriptorSetLayout m_materialDescriptorSetLayout;
		VkDescriptorSetLayout m_sceneDescriptorSetLayout;
		VkDescriptorSetLayout m_lightDescriptorSetLayout;

		std::vector<VkDescriptorSet> m_sceneDescriptorSets;
		std::vector<VkDescriptorSet> m_lightDescriptorSets;
    	//comands
		VkCommandPool m_commandPool;
		std::vector<VkCommandBuffer> m_commandBuffers;

    	//semaphores
		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;
		std::vector<VkFence> imagesInFlight;
		size_t currentFrame = 0;

		void createUniformBuffers();
		void createDescriptorPool();
		void createDescriptorSetLayout();
		void createSceneDescriptorSets();
    	
		void createCommandPool();
		void createCommandBuffers();
		void createSyncObjects();
		static uint32_t FindMemoryType(const VulkanContext& context, uint32_t typeFilter, VkMemoryPropertyFlags properties);

		void cleanupSwapchain();
		void cleanupDescriptorSetLayouts();
    };
}