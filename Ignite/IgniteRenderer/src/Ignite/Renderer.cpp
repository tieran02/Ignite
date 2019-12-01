#include "igpch.h"
#include "Ignite/Renderer.h"

#include <iostream>
#include <assert.h>
#include "Ignite/GraphicsContext.h"
#include "platform/Vulkan/VulkanRenderer.h"
#include "Ignite/Log.h"

namespace Ignite {
    Renderer::API Renderer::s_API = Renderer::API::VULKAN;

    std::unique_ptr<Renderer> Renderer::Create()
    {
        switch (s_API)
        {
            case Renderer::API::NONE:    CORE_ASSERT(false, "Renderer::NONE is currently not supported!"); return nullptr;
            case Renderer::API::VULKAN:  return std::make_unique<VulkanRenderer>();
        }
        return nullptr;
    }
}
