#pragma once
#include "Ignite/Renderer/IModel.h"
#include <vulkan/vulkan.h>

namespace Ignite
{
	class VulkanModel : public IModel
	{
		friend class IModel;
	protected:
		VulkanModel(const std::vector<float>& verts, const std::vector<uint16_t>& indices, const std::string& textureName);
		void Init(const std::vector<float>& verts, const std::vector<uint16_t>& indices, const std::string& textureName) override;
		void Cleanup() override;
	public:
		~VulkanModel() override;
	private:
		std::vector<VkDescriptorSet> m_descriptorSets;
		
		void createVBO(const std::vector<float>& verts);
		void createIndices(const std::vector<uint16_t>& indices);
		void createTexture2D(const std::string& textureName);
		
	public:
		void CreateDescriptorSet();
		void BindDescriptors() const override;
	};
}

