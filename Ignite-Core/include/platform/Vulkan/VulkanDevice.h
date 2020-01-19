#pragma once
#include "vulkan/vulkan.h"
#include "NonCopyable.h"

namespace Ignite
{
	class IWindow;

	struct QueueFamilyIndices {
		std::optional<uint32_t> m_graphicsFamily;

		bool isComplete() {
			return m_graphicsFamily.has_value();
		}
	};
	
	class VulkanDevice : NonCopyable
	{
	public:
		VulkanDevice(IWindow* window);
		~VulkanDevice();

	private:
		void create(IWindow* window);
		void cleanup();

		void createInstance();
		void pickPhysicalDevice();
		void createLogicalDevice();
		void createSurface(IWindow* window);
	private:
		const std::vector<const char*> m_validationLayers
		{
			"VK_LAYER_KHRONOS_validation"
		};

		const std::vector<const char*> m_gpuExtensions =
		{
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		const bool ENABLE_VALIDATION{ true };
		VkInstance m_instance{nullptr};
		VkDebugUtilsMessengerEXT m_debugMessenger;
		/** @brief Physical m_device representation */
		VkPhysicalDevice m_physicalDevice { nullptr };
		VkDevice m_device { nullptr };
		VkQueue m_graphicsQueue{ nullptr };
		VkSurfaceKHR m_surface{ nullptr };

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
