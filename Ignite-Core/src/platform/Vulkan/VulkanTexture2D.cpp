#include "igpch.h"
#include "platform/Vulkan/VulkanTexture2D.h"
#include "platform/Vulkan/VulkanBuffer.h"
#include "platform/Vulkan/VulkanContext.h"
#include "Ignite/Log.h"
#include <stb_image.h>
#include "platform/Vulkan/VulkanResources.h"


namespace Ignite
{
	VulkanTexture2D::VulkanTexture2D(const std::string& name, const std::string& path) : ITexture2D(name, path)
	{
		Init(path);
	}

	VulkanTexture2D::~VulkanTexture2D()
	{
		Cleanup();
	}

	void VulkanTexture2D::Free()
	{
		Cleanup();
	}


	void VulkanTexture2D::Init(const std::string& path)
	{
		createImage(path);
		createImageView();
		createTextureSampler();
	}

	void VulkanTexture2D::Cleanup()
	{
		if (m_deleted)
			return;

		LOG_CORE_INFO("Cleaning vulkan images: " + m_name);
		const VulkanContext* vulkanContext = reinterpret_cast<const VulkanContext*>(m_context);
		CORE_ASSERT(vulkanContext, "Failed to bind VulkanIndexBuffer, vulkan context is null");

		vkDestroySampler(vulkanContext->Device().LogicalDevice(), textureSampler, nullptr);
		vkDestroyImageView(vulkanContext->Device().LogicalDevice(), textureImageView, nullptr);

		vkDestroyImage(vulkanContext->Device().LogicalDevice(), m_textureImage, nullptr);
		vkFreeMemory(vulkanContext->Device().LogicalDevice(), m_textureImageMemory, nullptr);

		m_deleted = true;
	}

	void VulkanTexture2D::createTextureImageView()
	{
		const VulkanContext* vulkanContext = reinterpret_cast<const VulkanContext*>(m_context);
		CORE_ASSERT(vulkanContext, "Failed to bind VulkanIndexBuffer, vulkan context is null");
		
		textureImageView = createImageView();
	}

	VkImageView VulkanTexture2D::createImageView()
	{
		const VulkanContext* vulkanContext = reinterpret_cast<const VulkanContext*>(m_context);
		CORE_ASSERT(vulkanContext, "Failed to bind VulkanIndexBuffer, vulkan context is null");
		
		textureImageView = vulkanContext->Swapchain().CreateImageView(m_textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);

		return textureImageView;
	}

	void VulkanTexture2D::createTextureSampler()
	{
		const VulkanContext* vulkanContext = reinterpret_cast<const VulkanContext*>(m_context);
		CORE_ASSERT(vulkanContext, "Failed to bind VulkanIndexBuffer, vulkan context is null");
		
		VkSamplerCreateInfo samplerInfo = {};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = 16;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;

		VK_CHECK_RESULT(vkCreateSampler(vulkanContext->Device().LogicalDevice(), &samplerInfo, nullptr, &textureSampler));
	}

	void VulkanTexture2D::createImage(const std::string& path)
	{
		LOG_CORE_INFO("Creating vulkan images: " + m_name);
		
		const VulkanContext* vulkanContext = reinterpret_cast<const VulkanContext*>(m_context);
		CORE_ASSERT(vulkanContext, "Failed to bind VulkanIndexBuffer, vulkan context is null");

		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		m_width = texWidth;
		m_height = texHeight;
		CORE_ASSERT(pixels != nullptr, "failed to load texture image!");
		
		VkDeviceSize imageSize = m_width * m_height * 4;

		//image staging buffer
		VulkanBaseBuffer imageBuffer(vulkanContext);
		imageBuffer.CreateHostVisable(pixels, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

		//copy data to the buffer
		void* data;
		vkMapMemory(vulkanContext->Device().LogicalDevice(), imageBuffer.DeviceMemory(), 0, imageSize, 0, &data);
			memcpy(data, pixels, static_cast<size_t>(imageSize));
		vkUnmapMemory(vulkanContext->Device().LogicalDevice(), imageBuffer.DeviceMemory());

		//TODO abstract to image class
		stbi_image_free(pixels);

		VulkanResources::CreateImage(vulkanContext->Device().LogicalDevice(), vulkanContext->Device().PhysicalDevice(),
			m_width, m_height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_textureImage, m_textureImageMemory);

		VulkanResources::CopyBufferToImage(vulkanContext->Device().LogicalDevice(), vulkanContext->CommandPool(), vulkanContext->Device().GraphicsQueue(),
			imageBuffer.Buffer(), m_textureImage, static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height));
		
		imageBuffer.Free();

		m_deleted = false;
	}
}
