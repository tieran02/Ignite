#pragma once
#include "Ignite/Renderer/Material.h"
#include "VulkanPipeline.h"
#include "VulkanBuffer.h"

namespace Ignite
{
	class VulkanMaterial : public Material
	{
		friend class Material;
	protected:
		VulkanMaterial(const MaterialCreateInfo& materialInfo);
		
		void Init() override;
		void Cleanup() override;
	public:
		~VulkanMaterial() override;
		void Bind(const Pipeline* pipeline) const override;
		void Unbind(const Pipeline* pipeline) const override;

		void CreateDescriptorSets();
	private:
		// The material's descriptor contains the material descriptors
		std::vector<VkDescriptorSet> m_descriptorSets;
		std::vector<std::shared_ptr<VulkanBuffer>> m_propertyUniforms;
	};

	class NewVulkanMaterial : public BaseMaterial
	{
	public:

	private:
	};
}

