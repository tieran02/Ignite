#include "igpch.h"
#include <Ignite/IRendererAPI.h>
#include "Ignite/IGraphicsContext.h"
#include "Ignite/Log.h"
#include <platform\Vulkan\VulkanContext.h>

namespace Ignite
{
    std::unique_ptr<IGraphicsContext> IGraphicsContext::Create()
    {
        switch (IRendererAPI::GetAPI())
        {
            case IRendererAPI::API::NONE:    CORE_ASSERT(false, "IRendererAPI::NONE is currently not supported!"); return nullptr;
			case IRendererAPI::API::VULKAN:  return std::unique_ptr<VulkanContext>(new VulkanContext());
        }
            return std::unique_ptr<IGraphicsContext>();
    }
}