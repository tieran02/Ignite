#include "igpch.h"
#include "Ignite/Renderer/IMesh.h"
#include "Ignite/Log.h"
#include "Ignite/Renderer/Renderer.h"
#include "Ignite/Renderer/IRendererAPI.h"
#include "platform/Vulkan/VulkanMesh.h"
#include "Ignite/Renderer/Vertex.h"
#include "Ignite/Renderer/MeshData.h"

namespace Ignite
{
	IMesh::IMesh(const std::shared_ptr<IMaterial>& material) : m_context(Renderer::GraphicsContext()), m_matieral(material)
	{
	}

	std::shared_ptr<IMesh> IMesh::Create(const MeshData& meshData, const  std::shared_ptr<IMaterial>& material)
	{
		CORE_ASSERT(Renderer::IsInitialised(), "Failed to create buffer, Renderer is null")
			if (!meshData.HasData())
				return nullptr;

			switch (IRendererAPI::GetAPI())
			{
			case IRendererAPI::API::NONE:    CORE_ASSERT(false, "IRendererAPI::NONE is currently not supported!"); return nullptr;
			case IRendererAPI::API::VULKAN:
				std::shared_ptr<IMesh> model = std::shared_ptr<VulkanMesh>(new VulkanMesh(meshData, material));
				Renderer::GraphicsContext()->m_models.push_back(model);
				return model;
			}

		return nullptr;
	}
}
