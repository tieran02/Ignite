#pragma once
#include "VulkanDevice.h"

namespace Ignite
{
	class VulkanSwapChain
	{
	public:
		VulkanSwapChain(const VulkanDevice& device, uint32_t width, uint32_t height);
		~VulkanSwapChain();

		void Resize(uint32_t width, uint32_t height);

		uint32_t Width() const { return m_width; }
		uint32_t Height() const { return m_height; }

		const VkSurfaceFormatKHR& ImageFormat() const { return m_selectedFormat; }
		const std::vector<VkImage>& Images() const { return m_swapChainImages; }
		const std::vector<VkImageView>& ImageViews() const { return m_swapChainImageViews; }
		const VkSwapchainKHR& Swapchain() const { return m_swapchain; }

		//TODO Create image should go here aswell
		VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;
	private:
		uint32_t m_width, m_height;
		const VulkanDevice& m_device;
		VkSwapchainKHR m_swapchain{nullptr};

		VkSurfaceCapabilitiesKHR m_surfaceCapabilities;
		std::vector<VkPresentModeKHR> m_presentModes;
		std::vector<VkSurfaceFormatKHR> m_surfaceFormats;
		std::vector<VkImage> m_swapChainImages;

		VkSurfaceFormatKHR m_selectedFormat;
		VkPresentModeKHR m_selectedPresentMode;

		std::vector<VkImageView> m_swapChainImageViews;
		
		void create();
		void cleanup();
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
		void createImageViews();
	};
}

