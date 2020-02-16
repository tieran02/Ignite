#pragma once
#include "Ignite/Renderer/IMesh.h"
#include <vulkan/vulkan.h>

namespace Ignite
{
	class VulkanMesh : public IMesh
	{
		friend class IMesh;
	protected:
		VulkanMesh(const std::vector<Vertex>& verts, const std::vector<uint32_t>& indices, const std::vector<std::shared_ptr<ITexture2D>>& textures);
		void Init(const std::vector<Vertex>& verts, const std::vector<uint32_t>& indices, const std::vector<std::shared_ptr<ITexture2D>>& textures) override;
		void Cleanup() override;
	public:
		~VulkanMesh() override;
	private:
		std::vector<VkDescriptorSet> m_descriptorSets;
		
		void createVBO(const std::vector<Vertex>& verts);
		void createIndices(const std::vector<uint32_t>& indices);
		void createTextures(const std::vector<std::shared_ptr<ITexture2D>>& textures);
		
	public:
		void CreateDescriptorSet();
		void BindDescriptors() const override;
	};
}

