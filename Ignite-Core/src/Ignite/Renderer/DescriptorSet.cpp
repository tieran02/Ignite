#include "igpch.h"
#include "Ignite\Renderer\PipelineInputLayout.h"
#include "Ignite\Renderer\DescriptorSet.h"
#include "Ignite\Renderer\Renderer.h"

namespace Ignite 
{
	DescriptorSetLayout::DescriptorSetLayout() :
		IRegister(Renderer::GraphicsContext()->DescriptorSetLayouts()),
		m_type(SetType::NONE)
	{
		
	}

	DescriptorSetLayout::DescriptorSetLayout(SetType setType, StageBitSet stages) :
		IRegister(Renderer::GraphicsContext()->DescriptorSetLayouts()),
		m_type(setType),
		m_stages(stages)
	{
		Register();
	}

	DescriptorSetLayout::~DescriptorSetLayout()
	{
		Deregister();
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