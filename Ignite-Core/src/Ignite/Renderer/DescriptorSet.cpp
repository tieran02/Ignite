#include "igpch.h"
#include "Ignite\Renderer\PipelineInputLayout.h"
#include "Ignite\Renderer\DescriptorSet.h"
#include "Ignite\Renderer\Renderer.h"
#include "Ignite/Log.h"

namespace Ignite 
{
	DescriptorSetLayout::DescriptorSetLayout()
	{
		LOG_CORE_TRACE("Registered DescriptorSetLayout with UUID:{0}", m_uuid.Value());
		Renderer::GraphicsContext()->RegisterDescriptorSetLayout(*this);
	}

	DescriptorSetLayout::DescriptorSetLayout(SetType setType, StageBitSet stages) :
		m_type(setType),
		m_stages(stages)
	{
		LOG_CORE_TRACE("Registered DescriptorSetLayout with UUID:{0}", m_uuid.Value());
		Renderer::GraphicsContext()->RegisterDescriptorSetLayout(*this);
	}

	DescriptorSetLayout::~DescriptorSetLayout()
	{
		LOG_CORE_TRACE("Unregistered DescriptorSetLayout with UUID:{0}", m_uuid.Value());
		Renderer::GraphicsContext()->RegisterDescriptorSetLayout(*this, true);
	}

	void DescriptorSetLayout::SetStages(StageBitSet stages)
	{
		m_stages = stages;
	}

	bool DescriptorSetLayout::HasStage(SetBindingStage stage) const
	{
		return m_stages.test(to_underlying(stage));
	}

	StageBitSet DescriptorSetLayout::GetStages() const
	{
		return m_stages;
	}

	void DescriptorSetLayout::AddVariable(PipelineDataType variable)
	{
		m_variables.push_back(variable);
	}

	size_t DescriptorSetLayout::Size() const
	{
		size_t size{ 0 };
		for(const auto& var : m_variables)
		{
			size += PipelineDataTypeSize(var);
		}
		return size;
	}

	const UUID& DescriptorSetLayout::GetUUID() const
	{
		return m_uuid;
	}
}