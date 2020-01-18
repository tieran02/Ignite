#include "igpch.h"
#include <Ignite/Log.h>
#include "platform/Vulkan/VulkanContext.h"

namespace Ignite
{
	VulkanContext::VulkanContext() : IGraphicsContext()
    {
        Init();
    }

    VulkanContext::~VulkanContext()
    {
		Cleanup();
    }

    void VulkanContext::Init()
    {
        LOG_CORE_INFO("Initialising VulkanContext");
		m_vulkanDevice = std::make_unique<VulkanDevice>();
    }

	void VulkanContext::Cleanup()
	{
		m_vulkanDevice.reset();
		LOG_CORE_INFO("Cleaning VulkanContext");
	}

    void VulkanContext::SwapBuffers()
    {

    }
}