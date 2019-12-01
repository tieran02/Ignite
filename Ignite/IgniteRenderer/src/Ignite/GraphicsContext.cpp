#include "igpch.h"
#include <Ignite/Renderer.h>
#include "Ignite/GraphicsContext.h"
#include "Ignite/Log.h"

namespace Ignite
{
    std::unique_ptr<GraphicsContext> Ignite::GraphicsContext::Create()
    {
        switch (Renderer::GetAPI())
        {
            case Renderer::API::NONE:    CORE_ASSERT(false, "Renderer::NONE is currently not supported!"); return nullptr;
            case Renderer::API::VULKAN:  return nullptr;
        }
            return std::unique_ptr<GraphicsContext>();
    }
}