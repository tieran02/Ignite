#include "igpch.h"
#include <Ignite/Renderer/RendererAPI.h>
#include "Ignite/Renderer/GraphicsContext.h"
#include "Ignite/Log.h"
#include <platform\Vulkan\VulkanContext.h>
#include <Ignite/Renderer/Pipeline.h>
#include <Ignite/Renderer/Texture2D.h>
#include <Ignite/Renderer/Mesh.h>
#include <Ignite/Renderer/Material.h>

namespace Ignite
{
    std::unique_ptr<GraphicsContext> GraphicsContext::Create()
    {
        switch (RendererAPI::GetAPI())
        {
            case RendererAPI::API::NONE:    CORE_ASSERT(false, "IRendererAPI::NONE is currently not supported!"); return nullptr;
			case RendererAPI::API::VULKAN:  return std::unique_ptr<VulkanContext>(new VulkanContext());
        }
            return std::unique_ptr<GraphicsContext>();
    }

    const Pipeline* GraphicsContext::CreatePipeline(const PipelineCreateInfo& pipelineInfo)
    {
        bool exists = m_pipelines.find(pipelineInfo.GetName()) != m_pipelines.end();

    	if(exists)
    	{
            LOG_CORE_TRACE(pipelineInfo.GetName() + " Pipeline already attached to this context, using already attached pipeline");
            return m_pipelines[pipelineInfo.GetName()].get();
    	}
    	
        auto inserted = m_pipelines.insert(std::make_pair(pipelineInfo.GetName(), std::move(Pipeline::Create(pipelineInfo))));
        return inserted.first->second.get();
    }

    const Texture2D* GraphicsContext::CreateTexture2D(const Texture2DCreateInfo& textureInfo)
    {
        bool exists = m_texture2Ds.find(textureInfo.GetName()) != m_texture2Ds.end();

        if (exists)
        {
            LOG_CORE_TRACE(textureInfo.GetName() + " Texture2D already attached to this context, using already attached Texture2D");
            return m_texture2Ds[textureInfo.GetName()].get();
        }

        auto inserted = m_texture2Ds.insert(std::make_pair(textureInfo.GetName(), std::move(Texture2D::Create(textureInfo))));
        return inserted.first->second.get();

    }

    const Mesh* GraphicsContext::CreateMesh(const MeshCreateInfo& meshInfo)
    {
        bool exists = m_meshes.find(meshInfo.GetName()) != m_meshes.end();

        if (exists)
        {
            LOG_CORE_TRACE(meshInfo.GetName() + " Mesh already attached to this context, using already attached Mesh");
            return m_meshes[meshInfo.GetName()].get();
        }

        auto inserted = m_meshes.insert(std::make_pair(meshInfo.GetName(), std::move(Mesh::Create(meshInfo))));
        return inserted.first->second.get();
    }

    const Material* GraphicsContext::CreateMaterial(const MaterialCreateInfo& materialInfo)
    {
        bool exists = m_materials.find(materialInfo.GetName()) != m_materials.end();

        if (exists)
        {
            LOG_CORE_TRACE(materialInfo.GetName() + " Material already attached to this context, using already attached Material");
            return m_materials[materialInfo.GetName()].get();
        }

        auto inserted = m_materials.insert(std::make_pair(materialInfo.GetName(), std::move(Material::Create(materialInfo))));
        return inserted.first->second.get();
    }

    const Buffer* GraphicsContext::CreateBuffer(const BufferCreateInfo& bufferInfo)
    {
        bool exists = m_buffers.find(bufferInfo.GetName()) != m_buffers.end();

        if (exists)
        {
            LOG_CORE_TRACE(bufferInfo.GetName() + " Buffer already attached to this context, using already attached Buffer");
            return m_buffers[bufferInfo.GetName()].get();
        }

        auto inserted = m_buffers.insert(std::make_pair(bufferInfo.GetName(), std::move(Buffer::Create(bufferInfo))));
        return inserted.first->second.get();
    }
}
