#pragma once

#include <vulkan/vulkan_core.h>
#include "Ignite/Renderer/ITexture2D.h"

namespace  Ignite
{
	class VulkanDevice;
	class VulkanContext;

	class VulkanTexture2D : public ITexture2D
	{
		friend class ITexture2D;
	protected:
		VulkanTexture2D(const std::string& name, const std::string& path);
	public:
		~VulkanTexture2D() override;
		void Free() override;

		const VkImageView& ImageView()const { return textureImageView; }
		const VkSampler& Sampler()const { return textureSampler; }
	protected:
		void Init(const std::string& path) override;
		void Cleanup() override;

	private:
		void createImage(const std::string& path);
		void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
			VkImageUsageFlags usage,
			VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
		
		void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
		void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

		void createTextureImageView();
		VkImageView createImageView();

		void createTextureSampler();

		VkImage m_textureImage;
		VkDeviceMemory m_textureImageMemory;
		VkImageView textureImageView;
		VkSampler textureSampler;
	};
}
