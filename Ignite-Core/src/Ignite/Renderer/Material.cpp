#include "igpch.h"
#include "Ignite/Renderer/Material.h"
#include "Ignite/Renderer/Renderer.h"
#include "Ignite/Log.h"
#include "Ignite/Renderer/RendererAPI.h"
#include "platform/Vulkan/VulkanMaterial.h"


namespace Ignite
{
	Material::Material(const MaterialCreateInfo& materialInfo) : m_context(Renderer::GraphicsContext()), m_materialInfo(materialInfo)
	{
		if (m_materialInfo.DiffuseTexture() == nullptr)
		{
			if (m_context->Texture2Ds().find("default_diffuse") != m_context->Texture2Ds().end())
			{
				//get default texture
				m_materialInfo.m_diffuseTexture = m_context->Texture2Ds().at("default_diffuse");
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
				m_materialInfo.m_specularTexture = m_context->Texture2Ds().at("default_specular");
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
				m_materialInfo.m_normalTexture = m_context->Texture2Ds().at("default_normal");
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
				m_materialInfo.m_alphaTexture = m_context->Texture2Ds().at("default_diffuse");
			}
			else
			{
				LOG_CORE_FATAL("FAILED TO FIND DEFAULT ALPHA TEXTURE (default_specular)");
			}
		}
	}

	Ref<Material> Material::Create(const MaterialCreateInfo& materialInfo)
	{
		CORE_ASSERT(Renderer::IsInitialised(), "Failed to create buffer, Renderer is null")

			switch (RendererAPI::GetAPI())
			{
			case RendererAPI::API::NONE:    CORE_ASSERT(false, "IRendererAPI::NONE is currently not supported!"); return nullptr;
			case RendererAPI::API::VULKAN:	return Ref<VulkanMaterial>(new VulkanMaterial(materialInfo));
			}
		return nullptr;
	}

	Ref<Material> Material::DefaultMaterial()
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
