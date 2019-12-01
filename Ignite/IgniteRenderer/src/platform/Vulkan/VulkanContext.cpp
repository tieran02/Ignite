#include "igpch.h"
#include <Ignite/Log.h>
#include "platform/Vulkan/VulkanContext.h"

namespace Ignite
{
    VulkanContext::VulkanContext()
    {
        Init();
    }

    VulkanContext::~VulkanContext()
    {

    }

    void VulkanContext::Init()
    {
        LOG_CORE_INFO("Initialising VulkanContext");
    }

    void VulkanContext::SwapBuffers()
    {

    }
}