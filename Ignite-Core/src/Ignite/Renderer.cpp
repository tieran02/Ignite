#include "igpch.h"
#include "Ignite/IRenderer.h"

#include <iostream>
#include <assert.h>
#include "platform/Vulkan/VulkanRenderer.h"
#include "Ignite/Log.h"

namespace Ignite {
    IRenderer::API IRenderer::s_API = IRenderer::API::VULKAN;


	IRenderer::IRenderer()
	{
		m_graphicsContext = IGraphicsContext::Create();
	}

	std::unique_ptr<IRenderer> IRenderer::Create()
    {
        switch (s_API)
        {
            case IRenderer::API::NONE:    CORE_ASSERT(false, "IRenderer::NONE is currently not supported!"); return nullptr;
            case IRenderer::API::VULKAN:  return std::unique_ptr<VulkanRenderer>(new VulkanRenderer);
        }
        return nullptr;
    }

	IGraphicsContext& IRenderer::GetGraphicsContext()
	{
		return *m_graphicsContext;
	}

}
