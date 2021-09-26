#include "igpch.h"

#include "Ignite/Log.h"
#include "Ignite/Renderer/MaterialSystem.h"
#include "Ignite/Renderer/RendererAPI.h"
#include "Ignite/Renderer/GraphicsContext.h"
#include "Ignite/Renderer/Pipeline.h"

namespace Ignite {

	ShaderPass::ShaderPass(GraphicsContext& context, const ShaderEffect& effect) : m_effect(effect)
	{
		const std::string& vertex = effect.ShaderStageCode(ShaderStage::VERTEX);
		const std::string& fragment = effect.ShaderStageCode(ShaderStage::FRAGMENT);
		const std::string& geometry = effect.ShaderStageCode(ShaderStage::GEOMETRY);

		//TODO name is a hash of the paths, make UUID
		std::stringstream ss;
		ss << vertex << fragment << geometry;
		PipelineCreateInfo createInfo(ss.str(), effect.InputLayout(), vertex, fragment, geometry);

		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::NONE:    CORE_ASSERT(false, "IRendererAPI::NONE is currently not supported!"); break;
		case RendererAPI::API::VULKAN:
		{
			m_pipeline = context.CreatePipeline(createInfo);
			break;
		};
		}

		CORE_ASSERT(m_pipeline, "ShaderPass::ShaderPass failed to create pipeline");
	}

	ShaderEffect::ShaderEffect()
	{
		//TODO: pass in pipeline layout, for now just use this as default
		m_pipelineLayoutInfo = PipelineInputLayout
		{
			{ PipelineDataType::eFloat3, "a_Position" },
			{ PipelineDataType::eFloat3, "a_Normal" },
			{ PipelineDataType::eFloat4, "a_Tangent" },
			{ PipelineDataType::eFloat2, "a_TexCoord" }
		};
	}

	void ShaderEffect::LoadShaderStage(ShaderStage stage, const std::string & path)
	{
		CORE_ASSERT(stage != ShaderStage::COUNT, "ShaderEffect::ShaderStageCode cannot be ShaderStage::COUNT");
		CORE_ASSERT(m_stages[to_underlying(stage)].empty(), "ShaderEffect::ShaderStageCode stage already exists");

		m_stages[to_underlying(stage)] = path;
	}

	const PipelineInputLayout& ShaderEffect::InputLayout() const
	{
		return m_pipelineLayoutInfo;
	}
	const std::string & ShaderEffect::ShaderStageCode(ShaderStage stage) const
	{
		CORE_ASSERT(stage != ShaderStage::COUNT, "ShaderEffect::ShaderStageCode cannot be ShaderStage::COUNT");

		return m_stages[to_underlying(stage)];
	}

	EffectTemplate::EffectTemplate()
	{
		std::fill(m_passShaders.begin(), m_passShaders.end(), nullptr);
	}

	void EffectTemplate::SetShaderPass(ShaderPassType passType, ShaderPass * shaderPass)
	{
		CORE_ASSERT(passType < ShaderPassType::COUNT, "EffectTemplate::SetShaderPass invalid passType");
		m_passShaders[to_underlying(passType)] = shaderPass;
	}
}
