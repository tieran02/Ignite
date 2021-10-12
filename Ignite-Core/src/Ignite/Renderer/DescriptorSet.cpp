#include "igpch.h"
#include "Ignite\Renderer\PipelineInputLayout.h"
#include "Ignite\Renderer\DescriptorSet.h"

namespace Ignite 
{
	DescriptorSetLayout::DescriptorSetLayout(SetType setType, StageBitSet stages) :
		m_type(setType),
		m_stages(stages)
	{
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
}