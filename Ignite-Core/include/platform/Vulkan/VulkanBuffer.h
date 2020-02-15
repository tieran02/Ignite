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
		void CreateStaged(const void* data, VkDeviceSize size, VkBufferUsageFlags usage);
		void CreateHostVisable(const void* data, VkDeviceSize size, VkBufferUsageFlags usage);
		void Free();
		const VkBuffer& Buffer() const { return m_vkBuffer; }
		const VkDeviceMemory& DeviceMemory() const { return m_vkBufferMemory; }
	private:
		void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

		const VulkanContext* m_context;
		VkBuffer m_vkBuffer = nullptr;
		VkDeviceMemory m_vkBufferMemory = nullptr;
	};
	
	class VulkanBuffer : public IBuffer
	{
		friend class IBuffer;
	protected:
		VulkanBuffer(const void* data, size_t size);
	public:
		~VulkanBuffer() override;
	protected:
		void Init(const void* data, size_t size) override;
		void Cleanup() override;
	public:
		void Free() override;
		void Bind() const override;
		void Unbind() const override;
	private:
		VulkanBaseBuffer m_baseBuffer;
	};

	class VulkanVertexBuffer : public IVertexBuffer
	{
		friend class IVertexBuffer;
	protected:
		VulkanVertexBuffer(const float* data, size_t size);
	public:
		~VulkanVertexBuffer() override;
	protected:
		void Init(const void* data, size_t size) override;
		void Cleanup() override;
	public:
		void Free() override;
		void Bind() const override;
		void Unbind() const override;
	private:
		VulkanBaseBuffer m_baseBuffer;
	};

	class VulkanIndexBuffer : public IIndexBuffer
	{
		friend class IIndexBuffer;
	protected:
		VulkanIndexBuffer(const uint16_t* data, size_t size);
	public:
		~VulkanIndexBuffer() override;
	protected:
		void Init(const void* data, size_t size) override;
		void Cleanup() override;
	public:
		void Free() override;
		void Bind() const override;
		void Unbind() const override;
	private:
		VulkanBaseBuffer m_baseBuffer;
	};
}
