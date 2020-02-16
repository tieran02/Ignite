#include "igpch.h"
#include "Ignite/Renderer/IModel.h"
#include "Ignite/Log.h"
#include "Ignite/Renderer/Renderer.h"
#include "Ignite/Renderer/IRendererAPI.h"
#include "platform/Vulkan/VulkanModel.h"
#include "Ignite/Renderer/Vertex.h"

namespace Ignite
{
	IModel::IModel() : m_context(Renderer::GraphicsContext())
	{
	}

	std::shared_ptr<IModel> IModel::Create(const std::vector<Vertex>& verts, const std::vector<uint32_t>& indices, const std::string& textureName)
	{
		CORE_ASSERT(Renderer::IsInitialised(), "Failed to create buffer, Renderer is null")
			if (verts.size() == 0 || indices.size() == 0)
				return nullptr;

			switch (IRendererAPI::GetAPI())
			{
			case IRendererAPI::API::NONE:    CORE_ASSERT(false, "IRendererAPI::NONE is currently not supported!"); return nullptr;
			case IRendererAPI::API::VULKAN:
				std::shared_ptr<IModel> model = std::shared_ptr<VulkanModel>(new VulkanModel(verts, indices,textureName));
				Renderer::GraphicsContext()->m_models.push_back(model);
				return model;
			}

		return nullptr;
	}
}
