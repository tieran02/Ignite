#pragma once
#include "igpch.h"
#include "Ignite/Renderer/PipelineInputLayout.h"

namespace Ignite
{
	class GraphicsContext;
	class Pipeline;
	class Texture2D;

	enum class ShaderStage : uint8_t
	{
		VERTEX,
		FRAGMENT,
		GEOMETRY,
		COUNT
	};

	//Defines what types of resources can be accesses by a given pipeline, this includes uniform buffers
	struct DescriptorSet
	{
	public:
		
		enum class SetType : uint8_t
		{
			UNIFORM,
			SAMPLER,
			STORAGE,
			COUNT
		};

		struct SetLayout
		{
		public:
			std::vector<PipelineDataType> m_variables;
		private:
			SetType m_type;
			uint8_t m_stages; //What stages to expose this set in E.G (ShaderStage::FRAGMENT | ShaderStage::VERTEX) for frag and vertex 
		};

	public:

	private:
		std::vector<SetLayout> m_layouts;
	};

	struct ShaderEffect
	{
	public:
		ShaderEffect();
		void LoadShaderStage(ShaderStage stage, const std::string& path);

		const PipelineInputLayout& InputLayout() const;
		const std::string& ShaderStageCode(ShaderStage stage) const;
	private:
		PipelineInputLayout m_pipelineLayoutInfo;
		//This descriptor sets are only intended for data that is shared across all instances ( E.G light data/direction). 
		//Per instance sets such as images and light settings go into the material set
		DescriptorSet m_descriptorSets;
		std::array<std::string, to_underlying(ShaderStage::COUNT)> m_stages;
	};


	enum class ShaderPassType : uint8_t
	{
		FORWARD,
		TRANSPARENCY,
		SHADOWCAST,
		COUNT
	};

	enum class TransparencyMode : uint8_t
	{
		MODE_OPAQUE,
		MODE_TRANSPARENT,
		COUNT
	};

	//The shader pass is just the built version of the shader effect that contains the built pipeline
	struct ShaderPass
	{
	public:
		ShaderPass(GraphicsContext& context, const ShaderEffect& effect);
	private:
		const ShaderEffect m_effect;
		const Pipeline* m_pipeline;
	};

	struct EffectTemplate 
	{
		EffectTemplate();
		void SetShaderPass(ShaderPassType passType, ShaderPass* shaderPass);
	private:
		std::array<ShaderPass*, to_underlying(ShaderPassType::COUNT)> m_passShaders;
		//TODO default params
		TransparencyMode m_transparencyMode;
	};

	struct MaterialData
	{
	public:
	private:
		EffectTemplate* m_effect;
		std::array<DescriptorSet, to_underlying(ShaderPassType::COUNT)> m_passSets; //descriptor sets for per instance if a material, such as textures and specular settings
		std::vector<Texture2D*> m_textures;
	};
}