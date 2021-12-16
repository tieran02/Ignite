#pragma once
#include "igpch.h"

namespace Ignite
{
	class GraphicsContext;
	class Pipeline;
	class Texture2D;

	struct ShaderEffect : NonCopyable
	{
	public:
		ShaderEffect(std::vector<Ref<DescriptorSetLayout>>&& descriptorSets);
		ShaderEffect(ShaderEffect&& other);
		ShaderEffect& operator=(ShaderEffect&& other);


		void LoadShaderStage(SetBindingStage stage, const std::string& path);

		const PipelineInputLayout& InputLayout() const;
		const std::string& ShaderStageCode(SetBindingStage stage) const;
	private:
		PipelineInputLayout m_pipelineLayoutInfo;
		//This descriptor sets are only intended for data that is shared across all instances ( E.G light data/direction). 
		//Per instance sets such as images and light settings go into the material set
		std::vector<Ref<DescriptorSetLayout>> m_descriptorSets;
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
		ShaderPass(GraphicsContext& context, Ref<ShaderEffect> effect);
		ShaderPass(ShaderPass&& other);
		ShaderPass& operator=(ShaderPass&& other);

		const Ref<Pipeline>& GetPipeline() const;
	private:
		Ref<ShaderEffect> m_effect;
		Ref<Pipeline> m_pipeline;
	};

	struct EffectTemplate 
	{
		EffectTemplate();
		void SetShaderPass(ShaderPassType passType, Ref<ShaderPass> shaderPass);
		const std::array<Ref<ShaderPass>, to_underlying(ShaderPassType::COUNT)>& PassShaders() { return m_passShaders; };
	private:
		std::array<Ref<ShaderPass>, to_underlying(ShaderPassType::COUNT)> m_passShaders;
		//TODO default params
		TransparencyMode m_transparencyMode;
	};

	enum class MaterialParameterType
	{
		AMBIENT_COLOUR,
		DIFFUSE_COLOUR,
		SPEC_STRENGTH,
		NORMAL_STENGTH,
		COUNT,
	};

	template<MaterialParameterType> struct MaterialParamaterTypeMap;
	template<> struct MaterialParamaterTypeMap<MaterialParameterType::AMBIENT_COLOUR> { using type = glm::vec3; };
	template<> struct MaterialParamaterTypeMap<MaterialParameterType::DIFFUSE_COLOUR>	{ using type = glm::vec3; };
	template<> struct MaterialParamaterTypeMap<MaterialParameterType::SPEC_STRENGTH>	{ using type = float; };
	template<> struct MaterialParamaterTypeMap<MaterialParameterType::NORMAL_STENGTH>	{ using type = float; };

	template <MaterialParameterType T>
	using MaterialParameter = typename MaterialParamaterTypeMap<T>::type;

	struct BaseMaterialCreateInfo
	{
	public:
		std::string Name;
		std::array<std::string, to_underlying(TextureType::COUNT)> TextureNames;
		//TODO include the parameters in the create info, so it can be serialized at a later date
	};

	class BaseMaterial : public IRegister<BaseMaterial>
	{
	public:
		static Scope<BaseMaterial> Create(const BaseMaterialCreateInfo createInfo);

		template<MaterialParameterType T>
		void SetParameter(const MaterialParameter<T>& value);
		template<MaterialParameterType T>
		void ClearParameter();
		template<MaterialParameterType T>
		const MaterialParameter<T>* GetParameter() const;

		void SetTexture(TextureType textureType, Ref<Texture2D> texture2D);
	protected:
		BaseMaterial(const BaseMaterialCreateInfo createInfo);
	private:
		Ref<EffectTemplate> m_effect;
		//std::array<DescriptorSetLayout, to_underlying(ShaderPassType::COUNT)> m_passSets; //descriptor sets for per instance of a material, such as textures and specular settings
		std::array<Ref<Texture2D>, to_underlying(TextureType::COUNT)> m_textures;
		std::array<Ref<void>, to_underlying(MaterialParameterType::COUNT)> m_paramters{};
	};

	template<MaterialParameterType T>
	inline void BaseMaterial::SetParameter(const MaterialParameter<T>& value)
	{
		static_assert(T != MaterialParameterType::COUNT, "MaterialParameterType cannot be Count");
		ClearParameter<T>();
		m_paramters[to_underlying(T)] = CreateRef<MaterialParameter<T>>(value);
	}

	template<MaterialParameterType T>
	inline void BaseMaterial::ClearParameter()
	{
		static_assert(T != MaterialParameterType::COUNT, "Type cannot be Count!");
		m_paramters[to_underlying(T)].reset();
	}

	template<MaterialParameterType T>
	inline const MaterialParameter<T>* BaseMaterial::GetParameter() const
	{
		static_assert(T != MaterialParameterType::COUNT, "Type cannot be Count!");
		return static_cast<const MaterialParameter<T>*>(m_paramters[to_underlying(T)].get());
	}
}