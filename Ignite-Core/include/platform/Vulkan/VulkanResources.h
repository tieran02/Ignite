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
	};
}
