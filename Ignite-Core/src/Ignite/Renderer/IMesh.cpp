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
	IMesh::IMesh(const IMaterial* material) : m_context(Renderer::GraphicsContext()), m_matieral(material)
	{
	}

	std::shared_ptr<IMesh> IMesh::Create(const MeshCreateInfo& meshInfo)
	{
		CORE_ASSERT(Renderer::IsInitialised(), "Failed to create buffer, Renderer is null")
			if (!meshInfo.GetMeshData().HasData())
				return nullptr;

			switch (IRendererAPI::GetAPI())
			{
			case IRendererAPI::API::NONE:    CORE_ASSERT(false, "IRendererAPI::NONE is currently not supported!"); return nullptr;
			case IRendererAPI::API::VULKAN:
				std::shared_ptr<IMesh> model = std::shared_ptr<VulkanMesh>(new VulkanMesh(meshInfo));
				Renderer::GraphicsContext()->m_models.push_back(model);
				return model;
			}

		return nullptr;
	}
}
