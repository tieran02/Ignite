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
		VulkanTexture2D(const std::string& name, const std::string& path, TextureType textureType);
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
