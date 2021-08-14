#pragma once

#include <vulkan/vulkan_core.h>
#include "Ignite/Renderer/Texture2D.h"

namespace  Ignite
{
	class VulkanDevice;
	class VulkanContext;

	class VulkanTexture2D : public Texture2D
	{
		friend class Texture2D;
	protected:
		VulkanTexture2D(const Texture2DCreateInfo& info);
	public:
		~VulkanTexture2D() override;
		void Free() override;

		const VkImageView& ImageView()const { return textureImageView; }
		const VkSampler& Sampler()const { return textureSampler; }
	protected:
		void Init() override;
		void Cleanup() override;

	private:
		void createImage();

		void createTextureImageView();

		void createTextureSampler();

		VkFormat m_format;
		uint32_t m_mipLevels;
		VkImage m_textureImage;
		VkDeviceMemory m_textureImageMemory;
		VkImageView textureImageView;
		VkSampler textureSampler;
	};
}
