#include "igpch.h"
#include "Ignite/Renderer/PipelineInputLayout.h"
#include "Ignite/Log.h"


uint32_t Ignite::PipelineDataTypeSize(PipelineDataType type)
{
	switch (type)
	{
	case PipelineDataType::eFloat:    return 4;
	case PipelineDataType::eFloat2:   return 4 * 2;
	case PipelineDataType::eFloat3:   return 4 * 3;
	case PipelineDataType::eFloat4:   return 4 * 4;
	case PipelineDataType::eMat3:     return 4 * 3 * 3;
	case PipelineDataType::eMat4:     return 4 * 4 * 4;
	case PipelineDataType::eInt:      return 4;
	case PipelineDataType::eInt2:     return 4 * 2;
	case PipelineDataType::eInt3:     return 4 * 3;
	case PipelineDataType::eInt4:     return 4 * 4;
	case PipelineDataType::eBool:     return 1;
	default: break;
	}

	CORE_ASSERT(false, "Unknown PipelineDataType!");
	return 0;

}

Ignite::InputElement::InputElement(PipelineDataType type, const std::string& name, bool normalized)
	: Name(name), Type(type), Size(PipelineDataTypeSize(type)), Offset(0), Normalized(normalized)
{

}

uint32_t Ignite::InputElement::GetComponentCount() const
{
	switch (Type)
	{
	case PipelineDataType::eFloat:   return 1;
	case PipelineDataType::eFloat2:  return 2;
	case PipelineDataType::eFloat3:  return 3;
	case PipelineDataType::eFloat4:  return 4;
	case PipelineDataType::eMat3:    return 3 * 3;
	case PipelineDataType::eMat4:    return 4 * 4;
	case PipelineDataType::eInt:     return 1;
	case PipelineDataType::eInt2:    return 2;
	case PipelineDataType::eInt3:    return 3;
	case PipelineDataType::eInt4:    return 4;
	case PipelineDataType::eBool:    return 1;
	}
}

Ignite::PipelineInputLayout::PipelineInputLayout(const std::initializer_list<InputElement>& elements) : m_Elements(elements)
{
	CalculateOffsetsAndStride();
}

uint32_t Ignite::PipelineInputLayout::GetStride() const
{
	return m_Stride;
}

const std::vector<Ignite::InputElement>& Ignite::PipelineInputLayout::GetElements() const
{
	return m_Elements;
}

std::vector<Ignite::InputElement>::iterator Ignite::PipelineInputLayout::begin()
{
	return m_Elements.begin();
}

std::vector<Ignite::InputElement>::iterator Ignite::PipelineInputLayout::end()
{
	return m_Elements.end();
}

std::vector<Ignite::InputElement>::const_iterator Ignite::PipelineInputLayout::begin() const
{
	return m_Elements.begin();
}

std::vector<Ignite::InputElement>::const_iterator Ignite::PipelineInputLayout::end() const
{
	return m_Elements.end();
}

void Ignite::PipelineInputLayout::CalculateOffsetsAndStride()
{
	size_t offset = 0;
	m_Stride = 0;
	for (auto& element : m_Elements)
	{
		element.Offset = offset;
		offset += element.Size;
		m_Stride += element.Size;
	}
}
