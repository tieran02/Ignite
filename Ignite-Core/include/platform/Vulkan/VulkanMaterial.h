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
		VulkanMaterial(const std::string& name, const ITexture2D* diffuseTexture, const ITexture2D* specularTexture,
			const ITexture2D* normalTexture, const ITexture2D* alphaTexture);
		
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

