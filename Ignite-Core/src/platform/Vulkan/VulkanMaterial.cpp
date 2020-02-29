#include "igpch.h"
#include "platform/Vulkan/VulkanMaterial.h"
#include "platform/Vulkan/VulkanContext.h"
#include "Ignite/Log.h"
#include "platform/Vulkan/VulkanTexture2D.h"

Ignite::VulkanMaterial::VulkanMaterial(const std::string& name,
	const ITexture2D* diffuseTexture, const ITexture2D* specularTexture, 
	const ITexture2D* normalTexture, const ITexture2D* alphaTexture) : IMaterial(name,diffuseTexture,specularTexture, normalTexture,alphaTexture)
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

void Ignite::VulkanMaterial::Bind(const IPipeline* pipeline) const
{
	const VulkanContext* vulkanContext = reinterpret_cast<const VulkanContext*>(m_context);
	CORE_ASSERT(vulkanContext, "Failed to bind Descriptors, vulkan context is null");

	CORE_ASSERT(pipeline, "Failed to bind Descriptors, m_pipeline  is null");
	VkPipelineLayout layout = static_cast<const VulkanPipeline*>(pipeline)->PipelineLayout();

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

void Ignite::VulkanMaterial::Unbind(const IPipeline* pipeline) const
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
		
		VkDescriptorImageInfo diffuseImageInfo;
		//diffuse texture
		diffuseImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		const VulkanTexture2D& vulkanDiffuseImage = *(VulkanTexture2D*)m_diffuseTexture;
		diffuseImageInfo.imageView = vulkanDiffuseImage.ImageView();
		diffuseImageInfo.sampler = vulkanDiffuseImage.Sampler();


		VkWriteDescriptorSet diffuseDescriptorWrite{};
		diffuseDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		diffuseDescriptorWrite.dstSet = m_descriptorSets[i];
		diffuseDescriptorWrite.dstBinding = 0;
		diffuseDescriptorWrite.dstArrayElement = 0;
		diffuseDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		diffuseDescriptorWrite.descriptorCount = 1;
		diffuseDescriptorWrite.pImageInfo = &diffuseImageInfo;

		//specular texture
		VkDescriptorImageInfo specularImageInfo;
		specularImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		const VulkanTexture2D& vulkanSpecularImage = *(VulkanTexture2D*)m_specularTexture;
		specularImageInfo.imageView = vulkanSpecularImage.ImageView();
		specularImageInfo.sampler = vulkanSpecularImage.Sampler();

		VkWriteDescriptorSet specularDescriptorWrite{};
		specularDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		specularDescriptorWrite.dstSet = m_descriptorSets[i];
		specularDescriptorWrite.dstBinding = 1;
		specularDescriptorWrite.dstArrayElement = 0;
		specularDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		specularDescriptorWrite.descriptorCount = 1;
		specularDescriptorWrite.pImageInfo = &specularImageInfo;

		//normal texture
		VkDescriptorImageInfo normalImageInfo;
		normalImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		const VulkanTexture2D& vulkanNormalImage = *(VulkanTexture2D*)m_normalTexture;
		normalImageInfo.imageView = vulkanNormalImage.ImageView();
		normalImageInfo.sampler = vulkanNormalImage.Sampler();

		VkWriteDescriptorSet normalDescriptorWrite{};
		normalDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		normalDescriptorWrite.dstSet = m_descriptorSets[i];
		normalDescriptorWrite.dstBinding = 2;
		normalDescriptorWrite.dstArrayElement = 0;
		normalDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		normalDescriptorWrite.descriptorCount = 1;
		normalDescriptorWrite.pImageInfo = &normalImageInfo;

		//alpha texture
		VkDescriptorImageInfo alphaImageInfo;
		alphaImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		const VulkanTexture2D& vulkanAlphaImage = *(VulkanTexture2D*)m_alphaTexture;
		alphaImageInfo.imageView = vulkanAlphaImage.ImageView();
		alphaImageInfo.sampler = vulkanAlphaImage.Sampler();

		VkWriteDescriptorSet alphaDescriptorWrite{};
		alphaDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		alphaDescriptorWrite.dstSet = m_descriptorSets[i];
		alphaDescriptorWrite.dstBinding = 3;
		alphaDescriptorWrite.dstArrayElement = 0;
		alphaDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		alphaDescriptorWrite.descriptorCount = 1;
		alphaDescriptorWrite.pImageInfo = &alphaImageInfo;

		std::array<VkWriteDescriptorSet, 4> sets{ diffuseDescriptorWrite,specularDescriptorWrite,normalDescriptorWrite, alphaDescriptorWrite };

		vkUpdateDescriptorSets(vulkanContext->Device().LogicalDevice(), sets.size(), sets.data(), 0, nullptr);
	}
}
