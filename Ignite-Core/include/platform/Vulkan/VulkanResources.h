#pragma once
#include "vulkan/vulkan.h"

namespace Ignite
{
	class VulkanResources
	{
	public:
		static uint32_t FindMemoryType(VkPhysicalDevice physicalDevice ,uint32_t typeFilter, VkMemoryPropertyFlags properties);

		static VkCommandBuffer BeginSingleTimeCommands(VkDevice device, VkCommandPool commandPool);
		static void EndSingleTimeCommands(VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue, VkCommandBuffer commandBuffer);
		
		static void CreateBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		static void CopyBuffer(VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

		static void CreateImage(VkDevice device,VkPhysicalDevice physicalDevice, uint32_t width, uint32_t height, VkFormat format,
			VkImageTiling tiling,
			VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

		static void TransitionImageLayout(VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue, VkImage image, VkFormat format, VkImageLayout
		                                  oldLayout, VkImageLayout newLayout);
	};
}
