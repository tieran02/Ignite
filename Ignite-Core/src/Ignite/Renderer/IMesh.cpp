#include "igpch.h"
#include "Ignite/Renderer/IMesh.h"
#include "Ignite/Log.h"
#include "Ignite/Renderer/Renderer.h"
#include "Ignite/Renderer/IRendererAPI.h"
#include "platform/Vulkan/VulkanMesh.h"
#include "Ignite/Renderer/Vertex.h"

namespace Ignite
{
	IMesh::IMesh(const std::shared_ptr<IMaterial>& material) : m_context(Renderer::GraphicsContext()), m_matieral(material)
	{
	}

	std::shared_ptr<IMesh> IMesh::Create(const std::vector<Vertex>& verts, const std::vector<uint32_t>& indices,const  std::shared_ptr<IMaterial>& material)
	{
		CORE_ASSERT(Renderer::IsInitialised(), "Failed to create buffer, Renderer is null")
			if (verts.size() == 0 || indices.size() == 0)
				return nullptr;

			switch (IRendererAPI::GetAPI())
			{
			case IRendererAPI::API::NONE:    CORE_ASSERT(false, "IRendererAPI::NONE is currently not supported!"); return nullptr;
			case IRendererAPI::API::VULKAN:
				std::shared_ptr<IMesh> model = std::shared_ptr<VulkanMesh>(new VulkanMesh(verts, indices, material));
				Renderer::GraphicsContext()->m_models.push_back(model);
				return model;
			}

		return nullptr;
	}
}
