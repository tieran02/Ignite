#include "igpch.h"
#include <Ignite/Renderer/IPipeline.h>
#include "Ignite/Log.h"
#include "Ignite/Renderer/IRendererAPI.h"
#include "platform/Vulkan/VulkanPipeline.h"
#include "Ignite/Renderer/Renderer.h"

namespace Ignite
{
	IPipeline* IPipeline::s_currentBoundPipeline = nullptr;

	IPipeline::IPipeline(const PipelineInfo& info) : m_pipelineInfo(info), m_context(Renderer::GraphicsContext())
	{
	}

	std::shared_ptr<IPipeline> IPipeline::Create(const PipelineInfo& info)
	{
		CORE_ASSERT(Renderer::IsInitialised(), "Failed to create render pipeline, Renderer is null")
		
		switch (IRendererAPI::GetAPI())
		{
		case IRendererAPI::API::NONE:    CORE_ASSERT(false, "IRendererAPI::NONE is currently not supported!"); return nullptr;
		case IRendererAPI::API::VULKAN:
			if (Renderer::GraphicsContext()->m_pipelines.find(info.GetName()) == Renderer::GraphicsContext()->m_pipelines.end())
			{
				std::shared_ptr<VulkanPipeline> pipeline = std::shared_ptr<VulkanPipeline>(new VulkanPipeline(info));
				Renderer::GraphicsContext()->m_pipelines.insert(std::make_pair(info.GetName(), pipeline));
				return pipeline;
			}
			return Renderer::GraphicsContext()->m_pipelines.at(info.GetName());
		}
		
		return nullptr;
	}
}
