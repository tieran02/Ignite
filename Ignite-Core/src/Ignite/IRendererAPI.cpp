#include "igpch.h"
#include "Ignite/IRendererAPI.h"

#include <iostream>
#include <assert.h>
#include "platform/Vulkan/VulkanRendererAPI.h"
#include "Ignite/Log.h"

namespace Ignite {
    IRendererAPI::API IRendererAPI::s_API = IRendererAPI::API::VULKAN;


	IRendererAPI::IRendererAPI()
	{
		m_graphicsContext = IGraphicsContext::Create();
	}

	std::unique_ptr<IRendererAPI> IRendererAPI::Create()
    {
        switch (s_API)
        {
            case IRendererAPI::API::NONE:    CORE_ASSERT(false, "IRendererAPI::NONE is currently not supported!"); return nullptr;
            case IRendererAPI::API::VULKAN:  return std::unique_ptr<VulkanRendererAPI>(new VulkanRendererAPI());
        }
        return nullptr;
    }

	IGraphicsContext* IRendererAPI::GetGraphicsContext() const
	{
		return m_graphicsContext.get();
	}

}
