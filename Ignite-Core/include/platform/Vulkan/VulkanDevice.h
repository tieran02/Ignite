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
		const std::vector<const char*> validationLayers
		{
			"VK_LAYER_KHRONOS_validation"
		};

		const bool ENABLE_VALIDATION{ true };
		VkInstance instance{nullptr};
		VkDebugUtilsMessengerEXT debugMessenger;
		/** @brief Physical device representation */
		VkPhysicalDevice physicalDevice{ nullptr };

		const std::vector<const char*> getRequiredExtensions();
		const bool checkValidationLayerSupport() const;
		const void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		void setupDebugMessenger();
		VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
		void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
	};
}
