#include "igpch.h"
#include <Ignite/Renderer/Pipeline.h>
#include "Ignite/Log.h"
#include "Ignite/Renderer/RendererAPI.h"
#include "platform/Vulkan/VulkanPipeline.h"
#include "Ignite/Renderer/Renderer.h"

namespace Ignite
{
	Pipeline* Pipeline::s_currentBoundPipeline = nullptr;

	Pipeline::Pipeline(const PipelineCreateInfo& info) : m_pipelineInfo(info), m_context(Renderer::GraphicsContext())
	{
	}

	std::unique_ptr<Pipeline> Pipeline::Create(const PipelineCreateInfo& info)
	{
		CORE_ASSERT(Renderer::IsInitialised(), "Failed to create render pipeline, Renderer is null")
		
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::NONE:    CORE_ASSERT(false, "IRendererAPI::NONE is currently not supported!"); return nullptr;
		case RendererAPI::API::VULKAN:  return std::unique_ptr<VulkanPipeline>(new VulkanPipeline(info));
		}
		
		return nullptr;
	}
}
