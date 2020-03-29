#include "igpch.h"
#include <Ignite/Renderer/IPipeline.h>
#include "Ignite/Log.h"
#include "Ignite/Renderer/IRendererAPI.h"
#include "platform/Vulkan/VulkanPipeline.h"
#include "Ignite/Renderer/Renderer.h"

namespace Ignite
{
	IPipeline* IPipeline::s_currentBoundPipeline = nullptr;
	
	IPipeline::IPipeline(const std::string& name, const PipelineInputLayout& inputLayout, const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader) :
		m_context(Renderer::GraphicsContext()),
		m_inputLayout(inputLayout),
		m_name(name),
		m_vertexShader(vertexShader),
	    m_fragmentShader(fragmentShader),
		m_geometryShader(geometryShader)
	{

	}

	std::shared_ptr<IPipeline> IPipeline::Create(const std::string& name, const PipelineInputLayout& inputLayout, const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader)
	{
		CORE_ASSERT(Renderer::IsInitialised(), "Failed to create render pipeline, Renderer is null")
		
		switch (IRendererAPI::GetAPI())
		{
		case IRendererAPI::API::NONE:    CORE_ASSERT(false, "IRendererAPI::NONE is currently not supported!"); return nullptr;
		case IRendererAPI::API::VULKAN:
			if (Renderer::GraphicsContext()->m_pipelines.find(name) == Renderer::GraphicsContext()->m_pipelines.end()) 
			{
				std::shared_ptr<VulkanPipeline> pipeline = std::shared_ptr<VulkanPipeline>(new VulkanPipeline(name, inputLayout, vertexShader, fragmentShader,geometryShader));
				Renderer::GraphicsContext()->m_pipelines.insert(std::make_pair(name, pipeline));
				return pipeline;
			}
			return Renderer::GraphicsContext()->m_pipelines.at(name);
		}
		
		return nullptr;
	}
}
