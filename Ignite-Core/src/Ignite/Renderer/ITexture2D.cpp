#include "igpch.h"
#include "Ignite/Renderer/ITexture2D.h"
#include "Ignite/Renderer/Renderer.h"
#include "Ignite/Log.h"
#include "Ignite/Renderer/IRendererAPI.h"
#include "platform/Vulkan/VulkanTexture2D.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Ignite
{
	ITexture2D::ITexture2D(const Texture2DCreateInfo& info) : m_context(Renderer::GraphicsContext()), m_Texture2DInfo(info)
	{
	}

	std::unique_ptr<ITexture2D> ITexture2D::Create(const Texture2DCreateInfo& info)
	{
		CORE_ASSERT(Renderer::IsInitialised(), "Failed to create buffer, Renderer is null")

			switch (IRendererAPI::GetAPI())
			{
			case IRendererAPI::API::NONE:    CORE_ASSERT(false, "IRendererAPI::NONE is currently not supported!"); return nullptr;
			case IRendererAPI::API::VULKAN:	 return std::unique_ptr<VulkanTexture2D>(new VulkanTexture2D(info));
			}

		return nullptr;
	}
}
