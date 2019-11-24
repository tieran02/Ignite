#include "ignite/RendererAPI.h"

#include <iostream>
#include <assert.h>
#include "platform/vulkan/VulkanRenderAPI.h"
#include "ignite/Log.h"

namespace Ignite {
    RendererAPI::API RendererAPI::s_API = RendererAPI::API::VULKAN;

    std::unique_ptr<RendererAPI> RendererAPI::Create()
    {
        Log::Init();

        switch (s_API)
        {
            case RendererAPI::API::NONE:    CORE_ASSERT(false, "RendererAPI::NONE is currently not supported!"); return nullptr;
            case RendererAPI::API::VULKAN:  return std::make_unique<VulkanRenderAPI>();
        }
    }
}
