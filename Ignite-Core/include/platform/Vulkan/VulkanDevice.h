#pragma once
#include "vulkan/vulkan.h"
#include "NonCopyable.h"

namespace Ignite
{
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;

		bool isComplete() {
			return graphicsFamily.has_value();
		}
	};
	
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
		void createLogicalDevice();
	private:
		const std::vector<const char*> validationLayers
		{
			"VK_LAYER_KHRONOS_validation"
		};

		const std::vector<const char*> GpuExtensions =
		{
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		const bool ENABLE_VALIDATION{ true };
		VkInstance instance{nullptr};
		VkDebugUtilsMessengerEXT debugMessenger;
		/** @brief Physical device representation */
		VkPhysicalDevice physicalDevice{ nullptr };
		VkDevice device{ nullptr };
		VkQueue graphicsQueue;

		const std::vector<const char*> getRequiredExtensions();
		bool checkValidationLayerSupport() const;
		int rateDeviceSuitability(VkPhysicalDevice device);
		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice);
		
		void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		void setupDebugMessenger();
		VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
		void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
	};
}
