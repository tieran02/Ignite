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
		float opacity;
	};
	
	class IMaterial : NonCopyable
	{
	protected:
		IMaterial(const IPipeline* pipeline, const std::string& name, const ITexture2D* diffuseTexture);
		virtual void Init() = 0;
		virtual void Cleanup() = 0;
	public:
		virtual ~IMaterial() = default;
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		MaterialProperties& Properties() { return m_properties; }
		
		static std::shared_ptr<IMaterial> Create(const IPipeline* pipeline, const std::string& name, const ITexture2D* diffuseTexture);
	protected:
		const IGraphicsContext* m_context;

		const IPipeline* m_pipeline;
		std::string m_name;
		const ITexture2D* m_diffuseTexture;
		MaterialProperties m_properties;
	};
}
