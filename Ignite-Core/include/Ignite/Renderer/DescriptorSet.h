#pragma once
namespace Ignite
{
	enum class PipelineDataType;

	enum class SetBindingStage : uint8_t
	{
		VERTEX,
		FRAGMENT,
		GEOMETRY,
		COUNT
	};

	enum class SetType : uint8_t
	{
		UNIFORM_BUFFER,
		SAMPLER,
		STORAGE,
		COUNT
	};

	typedef std::bitset<to_underlying(SetBindingStage::COUNT)> StageBitSet;

	class DescriptorSetLayout
	{
	public:
		DescriptorSetLayout();
		DescriptorSetLayout(SetType setType, StageBitSet stages = StageBitSet{}.all());
		~DescriptorSetLayout();
		void SetStages(StageBitSet stages);
		bool HasStage(SetBindingStage stage) const;
		StageBitSet GetStages() const;

		void AddVariable(PipelineDataType variable);
		size_t Size() const;

		const UUID& GetUUID() const;
	private:
		UUID m_uuid;
		SetType m_type;
		StageBitSet m_stages; //What stages to expose this set in E.G (ShaderStage::FRAGMENT | ShaderStage::VERTEX) for frag and vertex 
		std::vector<PipelineDataType> m_variables;
	};
}