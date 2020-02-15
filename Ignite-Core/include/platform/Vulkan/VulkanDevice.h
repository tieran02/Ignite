#pragma once
#include "vulkan/vulkan.h"
#include "NonCopyable.h"

namespace Ignite
{
	class IWindow;

	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool isComplete() {
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};
	
	class VulkanDevice : NonCopyable
	{
	public:
		VulkanDevice();
		~VulkanDevice();

		const VkPhysicalDevice& PhysicalDevice() const { return m_physicalDevice; }
		const VkDevice& LogicalDevice() const { return m_device; }
		const VkSurfaceKHR& Surface() const { return m_surface; }
		const QueueFamilyIndices& QueueFamilies() const { return m_queueFamilies; }
		const VkQueue& GraphicsQueue() const { return m_graphicsQueue; }
		const VkQueue& PresentQueue() const { return m_presentQueue; }
		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;
		
	private:
		void create();
		void cleanup();

		void createInstance();
		void pickPhysicalDevice();
		void createLogicalDevice();
		void createSurface();
	private:
		const std::vector<const char*> m_validationLayers
		{
			"VK_LAYER_KHRONOS_validation",
			"VK_LAYER_LUNARG_monitor"
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
		VkQueue m_presentQueue{ nullptr };
		VkSurfaceKHR m_surface{ nullptr };
		QueueFamilyIndices m_queueFamilies;
		

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
