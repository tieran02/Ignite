#include "igpch.h"
#include "Ignite/Renderer/Mesh.h"
#include "Ignite/Log.h"
#include "Ignite/Renderer/Renderer.h"
#include "Ignite/Renderer/RendererAPI.h"
#include "platform/Vulkan/VulkanMesh.h"
#include "Ignite/Renderer/Vertex.h"
#include "Ignite/Renderer/MeshData.h"

namespace Ignite
{
	Mesh::Mesh(const MeshCreateInfo& meshInfo) :
		SceneObject(SceneObjectType::MESH),
		m_context(Renderer::GraphicsContext()),
		m_meshInfo(meshInfo)
	{
	}

	std::unique_ptr<Mesh> Mesh::Create(const MeshCreateInfo& meshInfo)
	{
		CORE_ASSERT(Renderer::IsInitialised(), "Failed to create buffer, Renderer is null")
			if (!meshInfo.GetMeshData().HasData())
				return nullptr;

			switch (RendererAPI::GetAPI())
			{
			case RendererAPI::API::NONE:    CORE_ASSERT(false, "IRendererAPI::NONE is currently not supported!"); return nullptr;
			case RendererAPI::API::VULKAN:  return std::unique_ptr<VulkanMesh>(new VulkanMesh(meshInfo));
			}

		return nullptr;
	}
}
