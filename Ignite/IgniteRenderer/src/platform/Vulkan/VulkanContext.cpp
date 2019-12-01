#include "igpch.h"
#include <Ignite/Log.h>
#include "platform/Vulkan/VulkanContext.h"

namespace Ignite
{
	VulkanContext::VulkanContext() : GraphicsContext()
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
    }

	void VulkanContext::Cleanup()
	{
		LOG_CORE_INFO("Cleaning VulkanContext");
	}

    void VulkanContext::SwapBuffers()
    {

    }
}