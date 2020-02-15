#include "igpch.h"
#include "platform/Vulkan/VulkanSwapChain.h"
#include "platform/Vulkan/VulkanContext.h"
#include "Ignite/Log.h"

namespace Ignite
{
	VulkanSwapChain::VulkanSwapChain(const VulkanDevice& device, uint32_t width, uint32_t height) : m_device(device), m_width(width), m_height(height)
	{
		create();
		createImageViews();
	}

	VulkanSwapChain::~VulkanSwapChain()
	{
		cleanup();
	}

	void VulkanSwapChain::Resize(uint32_t width, uint32_t height)
	{
		
	}

	VkImageView VulkanSwapChain::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) const
	{
		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = aspectFlags;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		VkImageView imageView;
		VK_CHECK_RESULT(vkCreateImageView(m_device.LogicalDevice(), &viewInfo, nullptr, &imageView));

		return imageView;
	}


	void VulkanSwapChain::create()
	{
		LOG_CORE_INFO("Creating vulkan swapchain");

		//get surface capabilities
		VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_device.PhysicalDevice(), m_device.Surface(), &m_surfaceCapabilities));

		//get present modes
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(m_device.PhysicalDevice(), m_device.Surface(), &presentModeCount, nullptr);
		CORE_ASSERT(presentModeCount, "FAILED CREATING SWAPCHAIN, NO PRESENT MODES DETECTED");
		m_presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(m_device.PhysicalDevice(), m_device.Surface(), &presentModeCount, m_presentModes.data());

		//get surface formats
		uint32_t formatsCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(m_device.PhysicalDevice(), m_device.Surface(), &formatsCount, nullptr);
		CORE_ASSERT(formatsCount, "FAILED CREATING SWAPCHAIN, NO FORMATS DETECTED");
		m_surfaceFormats.resize(formatsCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(m_device.PhysicalDevice(), m_device.Surface(), &formatsCount, m_surfaceFormats.data());

		
		if(m_surfaceFormats.size() == 1 && m_surfaceFormats[0].format == VK_FORMAT_UNDEFINED)
		{
			m_selectedFormat = { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
		}
		else
			m_selectedFormat = m_surfaceFormats[0];

		//TODO check if the user want to use VSync/triple buffer or not
		std::vector<std::pair<VkPresentModeKHR, unsigned int>> PresentModeRating
		{
				{ VK_PRESENT_MODE_IMMEDIATE_KHR   , 1 },
				{ VK_PRESENT_MODE_FIFO_RELAXED_KHR, 2 },
				{ VK_PRESENT_MODE_FIFO_KHR        , 3 },
				{ VK_PRESENT_MODE_MAILBOX_KHR     , 0 }
		};

		uint32_t bestRating = 0;
		for (auto& mode : m_presentModes) {
			for (auto rating : PresentModeRating) {
				if (mode == rating.first && rating.second > bestRating) {
					m_selectedPresentMode = mode;
					bestRating = rating.second;
				}
			}
		}

		uint32_t imageCount = m_surfaceCapabilities.minImageCount + 1;
		
		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = m_device.Surface();
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = m_selectedFormat.format;
		createInfo.imageColorSpace = m_selectedFormat.colorSpace;
		createInfo.imageExtent = chooseSwapExtent(m_surfaceCapabilities);
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		uint32_t queueFamilyIndices[] = { m_device.QueueFamilies().graphicsFamily.value(), m_device.QueueFamilies().presentFamily.value() };
		if (m_device.QueueFamilies().graphicsFamily != m_device.QueueFamilies().presentFamily) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0; // Optional
			createInfo.pQueueFamilyIndices = nullptr; // Optional
		}
		createInfo.preTransform = m_surfaceCapabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = m_selectedPresentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		VK_CHECK_RESULT(vkCreateSwapchainKHR(m_device.LogicalDevice(), &createInfo, nullptr, &m_swapchain));

		//get swap chain images
		vkGetSwapchainImagesKHR(m_device.LogicalDevice(), m_swapchain, &imageCount, nullptr);
		m_swapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(m_device.LogicalDevice(), m_swapchain, &imageCount, m_swapChainImages.data());
	}

	void VulkanSwapChain::cleanup()
	{
		LOG_CORE_INFO("Cleaning up vulkan swapchain");

		for (auto imageView : m_swapChainImageViews) {
			vkDestroyImageView(m_device.LogicalDevice(), imageView, nullptr);
		}
		
		if(m_swapchain != nullptr)
			vkDestroySwapchainKHR(m_device.LogicalDevice(), m_swapchain, nullptr);
	}

	VkExtent2D VulkanSwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
	{
		if (capabilities.currentExtent.width != UINT32_MAX) {
			return capabilities.currentExtent;
		}
		else {
			VkExtent2D actualExtent = { m_width, m_height };

			actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

			return actualExtent;
		}
	}

	void VulkanSwapChain::createImageViews()
	{
		m_swapChainImageViews.resize(m_swapChainImages.size());

		for (size_t i = 0; i < m_swapChainImages.size(); i++)
		{
			m_swapChainImageViews[i] = CreateImageView(m_swapChainImages[i], m_selectedFormat.format, VK_IMAGE_ASPECT_COLOR_BIT);
		}
	}
}
