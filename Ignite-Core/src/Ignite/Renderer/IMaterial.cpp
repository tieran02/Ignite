#include "igpch.h"
#include "Ignite/Renderer/IMaterial.h"
#include "Ignite/Renderer/Renderer.h"
#include "Ignite/Log.h"
#include "Ignite/Renderer/IRendererAPI.h"
#include "platform/Vulkan/VulkanMaterial.h"


namespace Ignite
{
	IMaterial::IMaterial(const MaterialCreateInfo& materialInfo) : m_context(Renderer::GraphicsContext()), m_materialInfo(materialInfo)
	{
		if (m_materialInfo.DiffuseTexture() == nullptr)
		{
			if (m_context->Texture2Ds().find("default_diffuse") != m_context->Texture2Ds().end())
			{
				//get default texture
				m_materialInfo.m_diffuseTexture = m_context->Texture2Ds().at("default_diffuse").get();
			}
			else
			{
				LOG_CORE_FATAL("FAILED TO FIND DEFAULT DIFFUSE TEXTURE (default_diffuse)");
			}
		}

		if (materialInfo.SpecularTexture() == nullptr)
		{
			if (m_context->Texture2Ds().find("default_specular") != m_context->Texture2Ds().end())
			{
				//get default texture
				m_materialInfo.m_specularTexture = m_context->Texture2Ds().at("default_specular").get();
			}
			else
			{
				LOG_CORE_FATAL("FAILED TO FIND DEFAULT SPECULAR TEXTURE (default_specular)");
			}
		}

		if (materialInfo.NormalTexture() == nullptr)
		{
			if (m_context->Texture2Ds().find("default_normal") != m_context->Texture2Ds().end())
			{
				//get default texture
				m_materialInfo.m_normalTexture = m_context->Texture2Ds().at("default_normal").get();
			}
			else
			{
				LOG_CORE_FATAL("FAILED TO FIND DEFAULT NORMAL TEXTURE (default_normal)");
			}
		}

		if (materialInfo.AlphaTexture() == nullptr)
		{
			if (m_context->Texture2Ds().find("default_diffuse") != m_context->Texture2Ds().end())
			{
				//get default texture
				m_materialInfo.m_alphaTexture = m_context->Texture2Ds().at("default_diffuse").get();
			}
			else
			{
				LOG_CORE_FATAL("FAILED TO FIND DEFAULT ALPHA TEXTURE (default_specular)");
			}
		}
	}

	std::unique_ptr<IMaterial> IMaterial::Create(const MaterialCreateInfo& materialInfo)
	{
		CORE_ASSERT(Renderer::IsInitialised(), "Failed to create buffer, Renderer is null")

			switch (IRendererAPI::GetAPI())
			{
			case IRendererAPI::API::NONE:    CORE_ASSERT(false, "IRendererAPI::NONE is currently not supported!"); return nullptr;
			case IRendererAPI::API::VULKAN: return std::unique_ptr<VulkanMaterial>(new VulkanMaterial(materialInfo));
			}
		return nullptr;
	}

	const IMaterial* IMaterial::DefaultMaterial()
	{
		MaterialCreateInfo createInfo
		{
			"default",
			MaterialProperties(),
			nullptr,
			nullptr,
			nullptr,
			nullptr
		};

		return Renderer::GraphicsContext()->CreateMaterial(createInfo);;
	}
}
