#include "igpch.h"
#include <Ignite/Renderer/IPipeline.h>
#include "Ignite/Log.h"
#include "Ignite/Renderer/IRendererAPI.h"
#include "platform/Vulkan/VulkanPipeline.h"
#include "Ignite/Renderer/Renderer.h"

namespace Ignite
{
	IPipeline::IPipeline(const std::string& vertexShader, const std::string& fragmentShader) :
		m_context(Renderer::GraphicsContext()),
		m_vertexShader(vertexShader),
	    m_fragmentShader(fragmentShader)
	{

	}

	std::unique_ptr<IPipeline> IPipeline::Create(const std::string& vertexShader, const std::string& fragmentShader)
	{
		CORE_ASSERT(Renderer::IsInitialised(), "Failed to create render pipeline, Renderer is null")
		
		switch (IRendererAPI::GetAPI())
		{
		case IRendererAPI::API::NONE:    CORE_ASSERT(false, "IRendererAPI::NONE is currently not supported!"); return nullptr;
		case IRendererAPI::API::VULKAN:  return std::unique_ptr<VulkanPipeline>(new VulkanPipeline(vertexShader, fragmentShader));
		}
		
		return nullptr;
	}
}
