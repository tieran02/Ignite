#pragma once
#include "Ignite/Renderer/IBuffer.h"
#include "vulkan/vulkan.h"

namespace Ignite
{
	class VulkanDevice;
	class VulkanContext;

	class VulkanBaseBuffer
	{
	public:
		VulkanBaseBuffer(const VulkanContext* context);
		void Create(const void* data, VkDeviceSize size, VkBufferUsageFlags usage, BUFFER_VISIBILITY visibility);
		void Free();
		void* Map();
		void Unmap();
		void Flush();
		
		const VkBuffer& Buffer() const { return m_vkBuffer; }
		const VkDeviceMemory& DeviceMemory() const { return m_vkBufferMemory; }
	private:
		const VulkanContext* m_context;
		VkBuffer m_vkBuffer = nullptr;
		VkDeviceMemory m_vkBufferMemory = nullptr;
		VkDeviceSize m_size;

		void CreateStaged(const void* data, VkDeviceSize size, VkBufferUsageFlags usage);
		void CreateHostVisable(const void* data, VkDeviceSize size, VkBufferUsageFlags usage);
	};
	
	class VulkanBuffer : public IBuffer
	{
		friend class IBuffer;
	protected:
		VulkanBuffer(const BufferCreateInfo& bufferInfo);
	public:
		~VulkanBuffer() override;
	protected:
		void Init() override;
		void Cleanup() override;
	public:
		void Free() override;
		void Bind() const override;
		void Unbind() const override;
		void Map() override;
		void Unmap() override;
		void Flush() override;
	private:
		VulkanBaseBuffer m_baseBuffer;
	};
}
