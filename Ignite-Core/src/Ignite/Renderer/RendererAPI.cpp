#include "igpch.h"
#include "Ignite/Renderer/RendererAPI.h"
#include "platform/Vulkan/VulkanRendererAPI.h"
#include "Ignite/Renderer/GraphicsContext.h"
#include "Ignite/Log.h"

namespace Ignite {
    RendererAPI::API RendererAPI::s_API = RendererAPI::API::VULKAN;


	RendererAPI::RendererAPI()
	{
		m_graphicsContext = GraphicsContext::Create();
	}

	std::unique_ptr<RendererAPI> RendererAPI::Create()
    {
        switch (s_API)
        {
            case RendererAPI::API::NONE:    CORE_ASSERT(false, "IRendererAPI::NONE is currently not supported!"); return nullptr;
            case RendererAPI::API::VULKAN:  return std::unique_ptr<VulkanRendererAPI>(new VulkanRendererAPI());
        }
        return nullptr;
    }

	GraphicsContext* RendererAPI::GetGraphicsContext() const
	{
		return m_graphicsContext.get();
	}

}
