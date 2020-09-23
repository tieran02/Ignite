#pragma once
#include <utility>


#include "glm/vec4.hpp"
#include "NonCopyable.h"


namespace Ignite
{
	class ITexture2D;
	class IPipeline;
	class IGraphicsContext;

	// Shader properites for a material
	// TODO Will be passed to the shaders using push constant 
	struct MaterialProperties
	{
		glm::vec4 ambient{ .35f,0.35f,0.35f,1.0f };
		glm::vec4 diffuse{ 1,1,1,1.0f };
		glm::vec4 specular{ 0,0,0,1.0f };
		float shininess{ 0.0f };
		float opacity{ 0.0f };
	};

	struct MaterialCreateInfo
	{
		friend class IMaterial;
	public:
		MaterialCreateInfo(const std::string& m_name,
			const MaterialProperties&& material_properties,
			const ITexture2D* m_diffuse_texture  = nullptr,
			const ITexture2D* m_specular_texture = nullptr,
			const ITexture2D* m_normal_texture   = nullptr,
			const ITexture2D* m_alpha_texture    = nullptr)
			: m_name(m_name),
			m_properties(std::move(material_properties)),
			m_diffuseTexture(m_diffuse_texture),
			m_specularTexture(m_specular_texture),
			m_normalTexture(m_normal_texture),
			m_alphaTexture(m_alpha_texture)
		{
		}

		const std::string& GetName() const { return m_name; }
		const MaterialProperties& GetMaterialProperties() const { return m_properties; }
		MaterialProperties& GetMaterialProperties() { return m_properties; }
		
		const ITexture2D* DiffuseTexture() const { return m_diffuseTexture; }
		const ITexture2D* SpecularTexture() const { return m_specularTexture; }
		const ITexture2D* NormalTexture() const { return m_normalTexture; }
		const ITexture2D* AlphaTexture() const { return m_alphaTexture; }
	private:
		const std::string m_name;
		MaterialProperties m_properties;
		const ITexture2D* m_diffuseTexture;
		const ITexture2D* m_specularTexture;
		const ITexture2D* m_normalTexture;
		const ITexture2D* m_alphaTexture;
	};
	
	class IMaterial : NonCopyable
	{
	protected:
		IMaterial(const MaterialCreateInfo& materialInfo);
		
		virtual void Init() = 0;
		virtual void Cleanup() = 0;
	public:
		virtual ~IMaterial() = default;
		virtual void Bind(const IPipeline* pipeline) const = 0;
		virtual void Unbind(const IPipeline* pipeline) const = 0;
		
		MaterialProperties& Properties() { return m_materialInfo.GetMaterialProperties(); }
		
		static std::unique_ptr<IMaterial> Create(const MaterialCreateInfo& materialInfo);
		static const IMaterial* DefaultMaterial();
	protected:
		const IGraphicsContext* m_context;
		MaterialCreateInfo m_materialInfo;
	};
}
