#include "igpch.h"
#include <Ignite/Renderer/IRendererAPI.h>
#include "Ignite/Renderer/IGraphicsContext.h"
#include "Ignite/Log.h"
#include <platform\Vulkan\VulkanContext.h>
#include <Ignite/Renderer/IPipeline.h>
#include <Ignite/Renderer/ITexture2D.h>
#include <Ignite/Renderer/IMesh.h>

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

    const IPipeline* IGraphicsContext::CreatePipeline(const PipelineCreateInfo& pipelineInfo)
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

    const ITexture2D* IGraphicsContext::CreateTexture2D(const Texture2DCreateInfo& textureInfo)
    {
        bool exists = m_texture2Ds.find(textureInfo.GetName()) != m_texture2Ds.end();
        CORE_ASSERT(!exists, "Texture2D already attached to this context");

        if (!exists)
        {
            auto inserted = m_texture2Ds.insert(std::make_pair(textureInfo.GetName(), std::move(ITexture2D::Create(textureInfo))));
            return inserted.first->second.get();
        }
        return m_texture2Ds[textureInfo.GetName()].get();
    }

    const IMesh* IGraphicsContext::CreateMesh(const MeshCreateInfo& meshInfo)
    {
        bool exists = m_meshes.find(meshInfo.GetName()) != m_meshes.end();
        CORE_ASSERT(!exists, "Mesh already attached to this context");

        if (!exists)
        {
            auto inserted = m_meshes.insert(std::make_pair(meshInfo.GetName(), std::move(IMesh::Create(meshInfo))));
            return inserted.first->second.get();
        }
        return m_meshes[meshInfo.GetName()].get();
    }

    const IMaterial* IGraphicsContext::CreateMaterial(const MaterialCreateInfo& materialInfo)
    {
        bool exists = m_materials.find(materialInfo.GetName()) != m_materials.end();
        CORE_ASSERT(!exists, "Material already attached to this context");

        if (!exists)
        {
            auto inserted = m_materials.insert(std::make_pair(materialInfo.GetName(), std::move(IMaterial::Create(materialInfo))));
            return inserted.first->second.get();
        }
        return m_materials[materialInfo.GetName()].get();
    }
}
