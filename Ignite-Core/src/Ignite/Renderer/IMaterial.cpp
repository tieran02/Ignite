#include "igpch.h"
#include "Ignite/Renderer/IMaterial.h"
#include "Ignite/Renderer/Renderer.h"
#include "Ignite/Log.h"
#include "Ignite/Renderer/IRendererAPI.h"
#include "platform/Vulkan/VulkanMaterial.h"

namespace Ignite
{
	IMaterial::IMaterial(const std::string& name, const ITexture2D* diffuseTexture, const ITexture2D* specularTexture, const ITexture2D* normalTexture, const ITexture2D* alphaTexture)
		: m_context(Renderer::GraphicsContext()),m_name(name), m_diffuseTexture(diffuseTexture), m_specularTexture(specularTexture), m_normalTexture(normalTexture), m_alphaTexture(alphaTexture)
	{
		m_properties = MaterialProperties
		{
			glm::vec4{.35f,0.35f,0.35f,1.0f},
			glm::vec4{1,1,1,1.0f},
			glm::vec4{0,0,0,1.0f},
			0.0f,
			0.0f
		};

		if (diffuseTexture == nullptr)
		{
			if (m_context->Texture2Ds().find("default_diffuse") != m_context->Texture2Ds().end())
			{
				//get default texture
				m_diffuseTexture = m_context->Texture2Ds().at("default_diffuse").get();
			}
			else
			{
				LOG_CORE_FATAL("FAILED TO FIND DEFAULT DIFFUSE TEXTURE (default_diffuse)");
			}
		}

		if (specularTexture == nullptr)
		{
			if (m_context->Texture2Ds().find("default_specular") != m_context->Texture2Ds().end())
			{
				//get default texture
				m_specularTexture = m_context->Texture2Ds().at("default_specular").get();
			}
			else
			{
				LOG_CORE_FATAL("FAILED TO FIND DEFAULT SPECULAR TEXTURE (default_specular)");
			}
		}

		if (normalTexture == nullptr)
		{
			if (m_context->Texture2Ds().find("default_normal") != m_context->Texture2Ds().end())
			{
				//get default texture
				m_normalTexture = m_context->Texture2Ds().at("default_normal").get();
			}
			else
			{
				LOG_CORE_FATAL("FAILED TO FIND DEFAULT NORMAL TEXTURE (default_normal)");
			}
		}

		if (alphaTexture == nullptr)
		{
			if (m_context->Texture2Ds().find("default_diffuse") != m_context->Texture2Ds().end())
			{
				//get default texture
				m_alphaTexture = m_context->Texture2Ds().at("default_diffuse").get();
			}
			else
			{
				LOG_CORE_FATAL("FAILED TO FIND DEFAULT ALPHA TEXTURE (default_specular)");
			}
		}
	}

	std::shared_ptr<IMaterial> IMaterial::Create(const std::string& name, const ITexture2D* diffuseTexture, const ITexture2D* specularTexture,
												 const ITexture2D* normalTexture, const ITexture2D* alphaTexture)
	{
		CORE_ASSERT(Renderer::IsInitialised(), "Failed to create buffer, Renderer is null")

			switch (IRendererAPI::GetAPI())
			{
			case IRendererAPI::API::NONE:    CORE_ASSERT(false, "IRendererAPI::NONE is currently not supported!"); return nullptr;
			case IRendererAPI::API::VULKAN:
				if (Renderer::GraphicsContext()->m_materials.find(name) == Renderer::GraphicsContext()->m_materials.end())
				{
					std::shared_ptr<IMaterial> material = std::shared_ptr<VulkanMaterial>(new VulkanMaterial(name, diffuseTexture, specularTexture, normalTexture, alphaTexture));
					Renderer::GraphicsContext()->m_materials.insert(std::make_pair(name, material));
					return material;
				}
				return Renderer::GraphicsContext()->m_materials.at(name);
			}

		return nullptr;
	}
}
