#include "igpch.h"
#include <Ignite/Renderer/IPipeline.h>
#include "Ignite/Log.h"
#include "Ignite/Renderer/IRendererAPI.h"
#include "platform/Vulkan/VulkanPipeline.h"
#include "Ignite/Renderer/Renderer.h"

namespace Ignite
{
	IPipeline* IPipeline::s_currentBoundPipeline = nullptr;

	IPipeline::IPipeline(const PipelineCreateInfo& info) : m_pipelineInfo(info), m_context(Renderer::GraphicsContext())
	{
	}

	std::unique_ptr<IPipeline> IPipeline::Create(const PipelineCreateInfo& info)
	{
		CORE_ASSERT(Renderer::IsInitialised(), "Failed to create render pipeline, Renderer is null")
		
		switch (IRendererAPI::GetAPI())
		{
		case IRendererAPI::API::NONE:    CORE_ASSERT(false, "IRendererAPI::NONE is currently not supported!"); return nullptr;
		case IRendererAPI::API::VULKAN:  return std::unique_ptr<VulkanPipeline>(new VulkanPipeline(info));
		}
		
		return nullptr;
	}
}
