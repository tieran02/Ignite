#pragma once
#include "vulkan/vulkan.h"
#include "NonCopyable.h"

namespace Ignite
{
	class VulkanDevice : NonCopyable
	{
	public:
		VulkanDevice();
		~VulkanDevice();

	private:
		void create();
		void cleanup();

		void createInstance();
		void pickPhysicalDevice();
	private:
		const bool ENABLE_VALIDATION{ true };
		VkInstance instance{nullptr};
		/** @brief Physical device representation */
		VkPhysicalDevice physicalDevice{ nullptr };
		
	};
}
