#include "igpch.h"
#include <Ignite/IRenderer.h>
#include "Ignite/IGraphicsContext.h"
#include "Ignite/Log.h"
#include <platform\Vulkan\VulkanContext.h>

namespace Ignite
{
    std::unique_ptr<IGraphicsContext> Ignite::IGraphicsContext::Create()
    {
        switch (IRenderer::GetAPI())
        {
            case IRenderer::API::NONE:    CORE_ASSERT(false, "IRenderer::NONE is currently not supported!"); return nullptr;
			case IRenderer::API::VULKAN:  return std::unique_ptr<VulkanContext>(new VulkanContext);
        }
            return std::unique_ptr<IGraphicsContext>();
    }
}