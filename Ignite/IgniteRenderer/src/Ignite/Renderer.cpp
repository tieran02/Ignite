#include "igpch.h"
#include "Ignite/Renderer.h"

#include <iostream>
#include <assert.h>
#include "platform/Vulkan/VulkanRenderer.h"
#include "Ignite/Log.h"

namespace Ignite {
    Renderer::API Renderer::s_API = Renderer::API::VULKAN;


	Renderer::Renderer()
	{
		m_graphicsContext = GraphicsContext::Create();
	}

	std::unique_ptr<Renderer> Renderer::Create()
    {
        switch (s_API)
        {
            case Renderer::API::NONE:    CORE_ASSERT(false, "Renderer::NONE is currently not supported!"); return nullptr;
            case Renderer::API::VULKAN:  return std::unique_ptr<VulkanRenderer>(new VulkanRenderer);
        }
        return nullptr;
    }

	GraphicsContext& Renderer::GetGraphicsContext()
	{
		return *m_graphicsContext;
	}

}
