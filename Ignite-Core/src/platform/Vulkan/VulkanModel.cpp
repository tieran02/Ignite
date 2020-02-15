#include "igpch.h"
#include "Ignite/Renderer/IGraphicsContext.h"
#include "platform/Vulkan/VulkanModel.h"
#include "Ignite/Log.h"
#include "platform/Vulkan/VulkanContext.h"
#include "platform/Vulkan/VulkanTexture2D.h"
#include "platform/Vulkan/VulkanPipeline.h"

namespace Ignite
{
	VulkanModel::VulkanModel(const std::vector<float>& verts,const std::vector<uint16_t>& indices, const std::string& textureName)
	{
		m_indexCount = indices.size();
		Init(verts, indices, textureName);
	}

	void VulkanModel::Init(const std::vector<float>& verts,const std::vector<uint16_t>& indices, const std::string& textureName)
	{
		createVBO(verts);
		createIndices(indices);
		createTexture2D(textureName);
		CreateDescriptorSet();
	}

	void VulkanModel::Cleanup()
	{
	}

	VulkanModel::~VulkanModel()
	{
		Cleanup();
	}

	void VulkanModel::createVBO(const std::vector<float>& verts)
	{
		m_vertexBuffer = IVertexBuffer::Create(verts.data(), sizeof(float) * verts.size());
	}

	void VulkanModel::createIndices(const std::vector<uint16_t>& indices)
	{
		m_IndexBuffer = IIndexBuffer::Create(indices.data(), sizeof(int) * indices.size());
	}

	void VulkanModel::createTexture2D(const std::string& textureName)
	{
		CORE_ASSERT(m_context->Texture2Ds().find(textureName) != m_context->Texture2Ds().end(), "Failed to find texture with name");
		
		m_image = m_context->Texture2Ds().at(textureName);
	}

	void VulkanModel::CreateDescriptorSet()
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
			VkDescriptorImageInfo imageInfo = {};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			const VulkanTexture2D& vulkanimage = *(VulkanTexture2D*)m_image.get();
			imageInfo.imageView = vulkanimage.ImageView();
			imageInfo.sampler = vulkanimage.Sampler();

			std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};

			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = m_descriptorSets[i];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &bufferInfo;

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = m_descriptorSets[i];
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pImageInfo = &imageInfo;

			vkUpdateDescriptorSets(vulkanContext->Device().LogicalDevice(), descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
		}
	}

	void VulkanModel::BindDescriptors() const
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
