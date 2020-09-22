#include "igpch.h"
#include <Ignite/Renderer/IRendererAPI.h>
#include "Ignite/Renderer/IGraphicsContext.h"
#include "Ignite/Log.h"
#include <platform\Vulkan\VulkanContext.h>
#include <Ignite/Renderer/IPipeline.h>

namespace Ignite
{
    std::unique_ptr<IGraphicsContext> IGraphicsContext::Create()
    {
        switch (IRendererAPI::GetAPI())
        {
            case IRendererAPI::API::NONE:    CORE_ASSERT(false, "IRendererAPI::NONE is currently not supported!"); return nullptr;
			case IRendererAPI::API::VULKAN:  return std::unique_ptr<VulkanContext>(new VulkanContext());
        }
            return std::unique_ptr<IGraphicsContext>();
    }

    IPipeline* IGraphicsContext::CreatePipeline(const PipelineCreateInfo& pipelineInfo)
    {
        bool exists = m_pipelines.find(pipelineInfo.GetName()) != m_pipelines.end();
        CORE_ASSERT(!exists, "Pipeline already attached to this context");
    	
        if (!exists)
        {
            auto inserted = m_pipelines.insert(std::make_pair(pipelineInfo.GetName(), std::move(IPipeline::Create(pipelineInfo))));
            return inserted.first->second.get();
        }
        return m_pipelines[pipelineInfo.GetName()].get();
    }
}
