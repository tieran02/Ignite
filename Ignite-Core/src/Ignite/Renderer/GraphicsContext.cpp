#include "igpch.h"
#include <Ignite/Renderer/RendererAPI.h>
#include "Ignite/Renderer/GraphicsContext.h"
#include "Ignite/Log.h"
#include <platform\Vulkan\VulkanContext.h>
#include <Ignite/Renderer/Pipeline.h>
#include <Ignite/Renderer/Texture2D.h>
#include <Ignite/Renderer/Mesh.h>
#include <Ignite/Renderer/Material.h>
#include "Ignite/ResourceFactory.h"

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
		return ResourceFactory::Create<Pipeline>(pipelineInfo, m_pipelines);
    }

    const Texture2D* GraphicsContext::CreateTexture2D(const Texture2DCreateInfo& textureInfo)
    {
		return ResourceFactory::Create<Texture2D>(textureInfo, m_texture2Ds);
    }

    const Mesh* GraphicsContext::CreateMesh(const MeshCreateInfo& meshInfo)
    {
		return ResourceFactory::Create<Mesh>(meshInfo, m_meshes);
    }

    const Material* GraphicsContext::CreateMaterial(const MaterialCreateInfo& materialInfo)
    {
		return ResourceFactory::Create<Material>(materialInfo, m_materials);
    }

    const Buffer* GraphicsContext::CreateBuffer(const BufferCreateInfo& bufferInfo)
    {
		return ResourceFactory::Create<Buffer>(bufferInfo, m_buffers);
    }
}