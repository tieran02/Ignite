#include "igpch.h"
#include "Ignite/Renderer/Texture2D.h"
#include "Ignite/Renderer/Renderer.h"
#include "Ignite/Log.h"
#include "Ignite/Renderer/RendererAPI.h"
#include "platform/Vulkan/VulkanTexture2D.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Ignite
{
	Texture2D::Texture2D(const Texture2DCreateInfo& info) : m_context(Renderer::GraphicsContext()), m_Texture2DInfo(info)
	{
	}

	std::unique_ptr<Texture2D> Texture2D::Create(const Texture2DCreateInfo& info)
	{
		CORE_ASSERT(Renderer::IsInitialised(), "Failed to create buffer, Renderer is null")

			switch (RendererAPI::GetAPI())
			{
			case RendererAPI::API::NONE:    CORE_ASSERT(false, "IRendererAPI::NONE is currently not supported!"); return nullptr;
			case RendererAPI::API::VULKAN:	 return std::unique_ptr<VulkanTexture2D>(new VulkanTexture2D(info));
			}

		return nullptr;
	}
}
