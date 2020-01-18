#include "igpch.h"
#include "platform/Vulkan/VulkanDevice.h"
#include "Ignite/Log.h"
#include "platform/Vulkan/VulkanContext.h"

Ignite::VulkanDevice::VulkanDevice()
{
	create();
}

Ignite::VulkanDevice::~VulkanDevice()
{
	cleanup();
}

void Ignite::VulkanDevice::create()
{
	LOG_CORE_INFO("Creating vulkan device");
	createInstance();
}

void Ignite::VulkanDevice::cleanup()
{
	LOG_CORE_INFO("Cleaning vulkan device");
	vkDestroyInstance(instance, nullptr);
}

void Ignite::VulkanDevice::createInstance()
{
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Ignite";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "Ignite";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	createInfo.enabledExtensionCount = 0;

	createInfo.enabledLayerCount = 0;

	VK_CHECK_RESULT(vkCreateInstance(&createInfo, nullptr, &instance));
}

void Ignite::VulkanDevice::pickPhysicalDevice()
{
	
}
