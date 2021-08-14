#pragma once
#include "Ignite/Renderer/Mesh.h"
#include <vulkan/vulkan.h>

namespace Ignite
{
	class VulkanMesh : public Mesh
	{
		friend class Mesh;
	protected:
		VulkanMesh(const MeshCreateInfo& meshInfo);
		void Init(const MeshData& meshData) override;
		void Cleanup() override;
	public:
		~VulkanMesh() override;
	private:
		std::vector<VkDescriptorSet> m_descriptorSets;
		
		void createVBO(const std::vector<Vertex>& verts);
		void createIndices(const std::vector<uint32_t>& indices);

	public:
		void CreateDescriptorSet();
		void BindDescriptors() const override;
	};
}

