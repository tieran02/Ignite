#include "igpch.h"
#include "platform/Vulkan/VulkanMaterial.h"
#include "platform/Vulkan/VulkanContext.h"
#include "Ignite/Log.h"
#include "platform/Vulkan/VulkanTexture2D.h"

Ignite::VulkanMaterial::VulkanMaterial(const IPipeline* pipeline, const std::string& name,
	const ITexture2D* diffuseTexture) : IMaterial(pipeline, name,diffuseTexture)
{
	Init();
}

void Ignite::VulkanMaterial::Init()
{
	CreateDescriptorSets();
}

void Ignite::VulkanMaterial::Cleanup()
{
}

Ignite::VulkanMaterial::~VulkanMaterial()
{
	Cleanup();
}

void Ignite::VulkanMaterial::Bind() const
{
	const VulkanContext* vulkanContext = reinterpret_cast<const VulkanContext*>(m_context);
	CORE_ASSERT(vulkanContext, "Failed to bind Descriptors, vulkan context is null");

	CORE_ASSERT(m_pipeline, "Failed to bind Descriptors, m_pipeline  is null");
	VkPipelineLayout layout = static_cast<const VulkanPipeline*>(m_pipeline)->PipelineLayout();

	for (size_t i = 0; i < vulkanContext->CommandBuffers().size(); i++)
	{
		vkCmdBindDescriptorSets(vulkanContext->CommandBuffers()[i], VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 1, 1,
			&m_descriptorSets[i], 0, nullptr);

		//Pass material properies via push constants
			vkCmdPushConstants(
				vulkanContext->CommandBuffers()[i],
				layout,
				VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(MaterialProperties),
				&m_properties);
	}

	
}

void Ignite::VulkanMaterial::Unbind() const
{
}

void Ignite::VulkanMaterial::CreateDescriptorSets()
{
	const VulkanContext* vulkanContext = reinterpret_cast<const VulkanContext*>(m_context);
	CORE_ASSERT(vulkanContext, "Failed to cleanup VulkanBuffer, vulkan context is null");

	std::vector<VkDescriptorSetLayout> layouts(vulkanContext->Swapchain().ImageViews().size(), vulkanContext->MaterialDescriptorSetLayout());
	
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = vulkanContext->DescriptorPool();
	allocInfo.descriptorSetCount = static_cast<uint32_t>(vulkanContext->Swapchain().ImageViews().size());
	allocInfo.pSetLayouts = layouts.data();

	m_descriptorSets.resize(vulkanContext->Swapchain().ImageViews().size());
	VK_CHECK_RESULT(vkAllocateDescriptorSets(vulkanContext->Device().LogicalDevice(), &allocInfo, m_descriptorSets.data()));

	for (size_t i = 0; i < vulkanContext->Swapchain().ImageViews().size(); i++)
	{
		
		VkDescriptorImageInfo imageInfos;

		//diffuse texture
		imageInfos.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		const VulkanTexture2D& vulkanimage = *(VulkanTexture2D*)m_diffuseTexture;
		imageInfos.imageView = vulkanimage.ImageView();
		imageInfos.sampler = vulkanimage.Sampler();


		VkWriteDescriptorSet diffuseDescriptorWrite{};
		diffuseDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		diffuseDescriptorWrite.dstSet = m_descriptorSets[i];
		diffuseDescriptorWrite.dstBinding = 0;
		diffuseDescriptorWrite.dstArrayElement = 0;
		diffuseDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		diffuseDescriptorWrite.descriptorCount = 1;
		diffuseDescriptorWrite.pImageInfo = &imageInfos;

		vkUpdateDescriptorSets(vulkanContext->Device().LogicalDevice(), 1, &diffuseDescriptorWrite, 0, nullptr);
	}
}
