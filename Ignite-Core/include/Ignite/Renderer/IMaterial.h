#pragma once
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
		glm::vec4 ambient;
		glm::vec4 diffuse;
		glm::vec4 specular;
		float shininess;
		float opacity;
	};
	
	class IMaterial : NonCopyable
	{
	protected:
		IMaterial(const std::string& name, const ITexture2D* diffuseTexture, const ITexture2D* specularTexture);
		virtual void Init() = 0;
		virtual void Cleanup() = 0;
	public:
		virtual ~IMaterial() = default;
		virtual void Bind(const IPipeline* pipeline) const = 0;
		virtual void Unbind(const IPipeline* pipeline) const = 0;
		MaterialProperties& Properties() { return m_properties; }
		
		static std::shared_ptr<IMaterial> Create(const std::string& name, const ITexture2D* diffuseTexture = nullptr, const ITexture2D* specularTexture = nullptr);
	protected:
		const IGraphicsContext* m_context;

		std::string m_name;
		const ITexture2D* m_diffuseTexture;
		const ITexture2D* m_specularTexture;
		MaterialProperties m_properties;
	};
}
