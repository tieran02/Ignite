#pragma once
#include <vector>

namespace Ignite
{
	enum class PipelineDataType
	{
		eNone = 0, eFloat, eFloat2, eFloat3, eFloat4, eMat3, eMat4, eInt, eInt2, eInt3, eInt4, eBool
	};

	static uint32_t PipelineDataTypeSize(PipelineDataType type);

	struct InputElement
	{
		std::string Name;
		PipelineDataType Type;
		uint32_t Size;
		size_t Offset;
		bool Normalized;

		InputElement() = default;

		InputElement(PipelineDataType type, const std::string& name, bool normalized = false);

		uint32_t GetComponentCount() const;
	};

	class PipelineInputLayout
	{
	public:
		PipelineInputLayout() = default;

		PipelineInputLayout(const std::initializer_list<InputElement>& elements);

		uint32_t GetStride() const;
		const std::vector<InputElement>& GetElements() const;

		std::vector<InputElement>::iterator begin();
		std::vector<InputElement>::iterator end();
		std::vector<InputElement>::const_iterator begin() const;
		std::vector<InputElement>::const_iterator end() const;

	private:
		void CalculateOffsetsAndStride();
		std::vector<InputElement> m_Elements;
		uint32_t m_Stride = 0;
	};
}
