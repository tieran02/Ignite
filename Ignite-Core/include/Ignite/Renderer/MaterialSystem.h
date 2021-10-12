#pragma once
#include "igpch.h"
#include "Ignite/Renderer/PipelineInputLayout.h"
#include "Ignite/Renderer/DescriptorSet.h"

namespace Ignite
{
	class GraphicsContext;
	class Pipeline;
	class Texture2D;

	struct ShaderEffect : NonCopyable
	{
	public:
		ShaderEffect() = default;
		ShaderEffect(std::vector<const DescriptorSetLayout*>&& descriptorSets);
		ShaderEffect(ShaderEffect&& other);
		ShaderEffect& operator=(ShaderEffect&& other);


		void LoadShaderStage(SetBindingStage stage, const std::string& path);

		const PipelineInputLayout& InputLayout() const;
		const std::string& ShaderStageCode(SetBindingStage stage) const;
	private:
		PipelineInputLayout m_pipelineLayoutInfo;
		//This descriptor sets are only intended for data that is shared across all instances ( E.G light data/direction). 
		//Per instance sets such as images and light settings go into the material set
		std::vector<const DescriptorSetLayout*> m_descriptorSets;
		std::array<std::string, to_underlying(SetBindingStage::COUNT)> m_stages;
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
	struct ShaderPass : NonCopyable
	{
	public:
		ShaderPass() = default;
		ShaderPass(GraphicsContext& context, ShaderEffect* effect);
		ShaderPass(ShaderPass&& other);
		ShaderPass& operator=(ShaderPass&& other);

		const Pipeline* GetPipeline() const;
	private:
		ShaderEffect* m_effect;
		Pipeline* m_pipeline;
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
		std::array<DescriptorSetLayout, to_underlying(ShaderPassType::COUNT)> m_passSets; //descriptor sets for per instance of a material, such as textures and specular settings
		std::vector<Texture2D*> m_textures;
	};
}