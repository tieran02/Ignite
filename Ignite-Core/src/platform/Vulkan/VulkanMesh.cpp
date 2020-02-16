#include "igpch.h"
#include "Ignite/Renderer/IGraphicsContext.h"
#include "platform/Vulkan/VulkanMesh.h"
#include "Ignite/Log.h"
#include "platform/Vulkan/VulkanContext.h"
#include "platform/Vulkan/VulkanTexture2D.h"
#include "platform/Vulkan/VulkanPipeline.h"

namespace Ignite
{
	VulkanMesh::VulkanMesh(const std::vector<Vertex>& verts, const std::vector<uint32_t>& indices, const std::vector<std::shared_ptr<ITexture2D>>& textures)
	{
		m_indexCount = indices.size();
		Init(verts, indices, textures);
	}

	void VulkanMesh::Init(const std::vector<Vertex>& verts, const std::vector<uint32_t>& indices, const std::vector<std::shared_ptr<ITexture2D>>& textures)
	{
		createVBO(verts);
		createIndices(indices);
		createTextures(textures);
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

	void VulkanMesh::createTextures(const std::vector<std::shared_ptr<ITexture2D>>& textures)
	{
		m_textures = textures;
	}

	void VulkanMesh::CreateDescriptorSet()
	{
		const VulkanContext* vulkanContext = reinterpret_cast<const VulkanContext*>(m_context);
		CORE_ASSERT(vulkanContext, "Failed to cleanup VulkanBuffer, vulkan context is null");
		
		std::vector<VkDescriptorSetLayout> layouts(vulkanContext->Swapchain().ImageViews().size(), vulkanContext->DescriptorSetLayout());
		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = vulkanContext->DescriptorPool();
		allocInfo.descriptorSetCount = static_cast<uint32_t>(vulkanContext->Swapchain().ImageViews().size());
		allocInfo.pSetLayouts = layouts.data();

		m_descriptorSets.resize(vulkanContext->Swapchain().ImageViews().size());
		VK_CHECK_RESULT(vkAllocateDescriptorSets(vulkanContext->Device().LogicalDevice(), &allocInfo, m_descriptorSets.data()));

		for (size_t i = 0; i < vulkanContext->Swapchain().ImageViews().size(); i++)
		{
			VkDescriptorBufferInfo bufferInfo = {};
			bufferInfo.buffer = vulkanContext->UniformBuffers()[i]->Buffer();
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(UniformBufferObject);


			//TODO have a model class with the descriptor set inside it, when we creae a model we create the descriptor set for it
			//todo get bound image
			std::vector<VkDescriptorImageInfo> imageInfos{ m_textures.size() };
			for (size_t i = 0; i < m_textures.size(); i++)
			{
				imageInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				const VulkanTexture2D& vulkanimage = *(VulkanTexture2D*)m_textures[i].get();
				imageInfos[i].imageView = vulkanimage.ImageView();
				imageInfos[i].sampler = vulkanimage.Sampler();
			}

			std::vector<VkWriteDescriptorSet> descriptorWrites;

			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = m_descriptorSets[i];
			descriptorWrite.dstBinding = 0;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &bufferInfo;
			descriptorWrites.push_back(descriptorWrite);

			if (!m_textures.empty())
			{
				VkWriteDescriptorSet imageDescriptorWrite{};
				imageDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				imageDescriptorWrite.dstSet = m_descriptorSets[i];
				imageDescriptorWrite.dstBinding = 1;
				imageDescriptorWrite.dstArrayElement = 0;
				imageDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				imageDescriptorWrite.descriptorCount = imageInfos.size();
				imageDescriptorWrite.pImageInfo = imageInfos.data();
				descriptorWrites.push_back(imageDescriptorWrite);
			}

			vkUpdateDescriptorSets(vulkanContext->Device().LogicalDevice(), descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
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
