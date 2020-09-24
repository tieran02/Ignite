#pragma once
#include "Ignite/Renderer/IBuffer.h"
#include "vulkan/vulkan.h"

namespace Ignite
{
	class VulkanDevice;
	class VulkanContext;

	//class VulkanBaseBuffer
	//{
	//public:
	//	VulkanBaseBuffer(const VulkanContext* context);
	//	void Create(const void* data, VkDeviceSize size, VkBufferUsageFlags usage, BUFFER_VISIBILITY visibility);
	//	void Free();
	//	void* Map();
	//	void Unmap();
	//	void Flush();

	//	const VkBuffer& Buffer() const { return m_vkBuffer; }
	//	const VkDeviceMemory& DeviceMemory() const { return m_vkBufferMemory; }
	//private:
	//	const VulkanContext* m_context;
	//	VkBuffer m_vkBuffer = nullptr;
	//	VkDeviceMemory m_vkBufferMemory = nullptr;
	//	VkDeviceSize m_size;

	//	void CreateStaged(const void* data, VkDeviceSize size, VkBufferUsageFlags usage);
	//	void CreateHostVisable(const void* data, VkDeviceSize size, VkBufferUsageFlags usage);
	//};
	
	class VulkanBuffer : public IBuffer
	{
		friend class IBuffer;
	public:
		VulkanBuffer(const BufferCreateInfo& bufferInfo, const VulkanContext& context);
		~VulkanBuffer() override;

		VulkanBuffer(VulkanBuffer&& other) noexcept
			: IBuffer(std::move(other.m_bufferInfo)),
		      m_vulkanContext(other.m_vulkanContext),
			  m_vkBuffer(other.m_vkBuffer),
			  m_vkBufferMemory(other.m_vkBufferMemory),
			  m_size(other.m_size)
		{
			other.m_vkBuffer = VK_NULL_HANDLE;
			other.m_vkBufferMemory = VK_NULL_HANDLE;
			other.m_size = 0;
		}


	protected:
		void Init() override;
		void Cleanup() override;
	public:
		void Free() override;
		void Bind() const override;
		void Unbind() const override;
		void* Map() const override;
		void Unmap() const override;
		void Flush() const override;
		void CopyToBuffer(void* data, size_t size) override;

		VkBuffer Buffer() const { return m_vkBuffer; }
		VkDeviceMemory DeviceMemory() const { return m_vkBufferMemory; }
	private:
		const VulkanContext& m_vulkanContext;
		VkBuffer m_vkBuffer = VK_NULL_HANDLE;
		VkDeviceMemory m_vkBufferMemory = VK_NULL_HANDLE;
		VkDeviceSize m_size;

		void Create(const void* data, VkDeviceSize size, VkBufferUsageFlags usage, BUFFER_VISIBILITY visibility);
		void CreateStaged(const void* data, VkDeviceSize size, VkBufferUsageFlags usage);
		void CreateHostVisable(const void* data, VkDeviceSize size, VkBufferUsageFlags usage);
	};
}
