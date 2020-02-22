#include "igpch.h"
#include "Ignite/Renderer/IGraphicsContext.h"
#include "platform/Vulkan/VulkanMesh.h"
#include "Ignite/Log.h"
#include "platform/Vulkan/VulkanContext.h"
#include "platform/Vulkan/VulkanTexture2D.h"
#include "platform/Vulkan/VulkanPipeline.h"

namespace Ignite
{
	VulkanMesh::VulkanMesh(const std::vector<Vertex>& verts, const std::vector<uint32_t>& indices, std::shared_ptr<IMaterial> material) : IMesh(material)
	{
		m_indexCount = indices.size();
		Init(verts, indices);
	}

	void VulkanMesh::Init(const std::vector<Vertex>& verts, const std::vector<uint32_t>& indices)
	{
		createVBO(verts);
		createIndices(indices);
		CreateDescriptorSet();
	}

	void VulkanMesh::Cleanup()
	{
	}

	VulkanMesh::~VulkanMesh()
	{
		Cleanup();
	}

	void VulkanMesh::createVBO(const std::vector<Vertex>& verts)
	{
		m_vertexBuffer = IVertexBuffer::Create(verts.data(), sizeof(verts[0]) * verts.size());
	}

	void VulkanMesh::createIndices(const std::vector<uint32_t>& indices)
	{
		m_IndexBuffer = IIndexBuffer::Create(indices.data(), sizeof(indices[0]) * indices.size());
	}


	void VulkanMesh::CreateDescriptorSet()
	{
		const VulkanContext* vulkanContext = reinterpret_cast<const VulkanContext*>(m_context);
		CORE_ASSERT(vulkanContext, "Failed to create descriptorsets, vulkan context is null");
		
		std::vector<VkDescriptorSetLayout> layouts(vulkanContext->Swapchain().ImageViews().size(), vulkanContext->SceneDescriptorSetLayout());
		
		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = vulkanContext->DescriptorPool();
		allocInfo.descriptorSetCount = static_cast<uint32_t>(vulkanContext->Swapchain().ImageViews().size());
		allocInfo.pSetLayouts = layouts.data();

		m_descriptorSets.resize(vulkanContext->Swapchain().ImageViews().size());
		VkResult r = vkAllocateDescriptorSets(vulkanContext->Device().LogicalDevice(), &allocInfo, m_descriptorSets.data());

		for (size_t i = 0; i < vulkanContext->Swapchain().ImageViews().size(); i++)
		{
			VkDescriptorBufferInfo bufferInfo = {};
			bufferInfo.buffer = vulkanContext->UniformBuffers()[i]->Buffer();
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(UniformBufferObject);

			VkWriteDescriptorSet descriptorWrite = {};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = m_descriptorSets[i];
			descriptorWrite.dstBinding = 0;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &bufferInfo;

			std::array<VkWriteDescriptorSet, 1> write_descriptors{ descriptorWrite };
			vkUpdateDescriptorSets(vulkanContext->Device().LogicalDevice(), write_descriptors.size(), write_descriptors.data(), 0, nullptr);
		}
	}

	void VulkanMesh::BindDescriptors() const
	{
		const VulkanContext* vulkanContext = reinterpret_cast<const VulkanContext*>(m_context);
		CORE_ASSERT(vulkanContext, "Failed to bind Descriptors, vulkan context is null");

		const IPipeline* pipeline = IPipeline::CurrentBoundPipeline();
		CORE_ASSERT(pipeline, "Failed to cleanup VulkanBuffer, vulkan context is null");
		VkPipelineLayout layout = static_cast<const VulkanPipeline*>(pipeline)->PipelineLayout();

		for (size_t i = 0; i < vulkanContext->CommandBuffers().size(); i++)
		{
			vkCmdBindDescriptorSets(vulkanContext->CommandBuffers()[i], VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1,
				&m_descriptorSets[i], 0, nullptr);
		}
	}
}
