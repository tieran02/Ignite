#pragma once
#include "Ignite/Renderer/IMaterial.h"
#include "VulkanPipeline.h"
#include "VulkanBuffer.h"

namespace Ignite
{
	class VulkanMaterial : public IMaterial
	{
		friend class IMaterial;
	protected:
		VulkanMaterial(const MaterialCreateInfo& materialInfo);
		
		void Init() override;
		void Cleanup() override;
	public:
		~VulkanMaterial() override;
		void Bind(const IPipeline* pipeline) const override;
		void Unbind(const IPipeline* pipeline) const override;

		void CreateDescriptorSets();
	private:
		// The material's descriptor contains the material descriptors
		std::vector<VkDescriptorSet> m_descriptorSets;
		std::vector<std::shared_ptr<VulkanBuffer>> m_propertyUniforms;
	};
}

