#include "igpch.h"
#include "platform/Vulkan/VulkanBuffer.h"
#include "vulkan/vulkan.h"
#include "platform/Vulkan/VulkanContext.h"
#include "Ignite/Log.h"
#include "platform/Vulkan/VulkanResources.h"

namespace Ignite
{
	VulkanBaseBuffer::VulkanBaseBuffer(const VulkanContext* context) : m_context(context)
	{}

	void VulkanBaseBuffer::CreateStaged(const void* data, VkDeviceSize size, VkBufferUsageFlags usage)
	{
		//staging buffer
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		
		VulkanResources::CreateBuffer(m_context->Device().LogicalDevice(), m_context->Device().PhysicalDevice(),size,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer, stagingBufferMemory);

		void* bufferData = nullptr;
		vkMapMemory(m_context->Device().LogicalDevice(), stagingBufferMemory, 0, size, 0, &bufferData);
		memcpy(bufferData, data, (size_t)size);
		vkUnmapMemory(m_context->Device().LogicalDevice(), stagingBufferMemory);

		//data buffer
		VulkanResources::CreateBuffer(m_context->Device().LogicalDevice(), m_context->Device().PhysicalDevice(), size,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			m_vkBuffer, m_vkBufferMemory);
		

		VulkanResources::CopyBuffer(m_context->Device().LogicalDevice(), m_context->CommandPool(), m_context->Device().GraphicsQueue(),
			stagingBuffer, m_vkBuffer, size);

		vkDestroyBuffer(m_context->Device().LogicalDevice(), stagingBuffer, nullptr);
		vkFreeMemory(m_context->Device().LogicalDevice(), stagingBufferMemory, nullptr);
	}

	void VulkanBaseBuffer::CreateHostVisable(const void* data, VkDeviceSize size, VkBufferUsageFlags usage)
	{
		VulkanResources::CreateBuffer(m_context->Device().LogicalDevice(), m_context->Device().PhysicalDevice(), size,
			usage, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			m_vkBuffer, m_vkBufferMemory);
	}

	void VulkanBaseBuffer::Free()
	{
		if (m_vkBuffer != nullptr)
		{
			vkDestroyBuffer(m_context->Device().LogicalDevice(), m_vkBuffer, nullptr);
			m_vkBuffer = nullptr;
		}
		if (m_vkBufferMemory != nullptr)
		{
			vkFreeMemory(m_context->Device().LogicalDevice(), m_vkBufferMemory, nullptr);
			m_vkBufferMemory = nullptr;
		}
	}

	VulkanBuffer::VulkanBuffer(const void* data, size_t size) : m_baseBuffer(VulkanBaseBuffer(reinterpret_cast<const VulkanContext*>(m_context)))
	{
		Init(data, size);
	}

	VulkanBuffer::~VulkanBuffer()
	{
		Cleanup();
	}

	void VulkanBuffer::Init(const void* data, size_t size)
	{
		m_baseBuffer.CreateStaged(data, size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
		m_deleted = false;
	}

	void VulkanBuffer::Cleanup()
	{
		const VulkanContext* vulkanContext = reinterpret_cast<const VulkanContext*>(m_context);
		CORE_ASSERT(vulkanContext, "Failed to cleanup VulkanBuffer, vulkan context is null");

		if (!m_deleted)
		{
			m_baseBuffer.Free();
			m_deleted = true;
		}
	}

	void VulkanBuffer::Free()
	{
		Cleanup();
	}

	void VulkanBuffer::Bind() const
	{
		//TODO IMPLEMENT BIND STORAGE BUFFER
	}

	void VulkanBuffer::Unbind() const
	{
		//TODO IMPLEMENT BIND STORAGE BUFFER
	}



	VulkanVertexBuffer::VulkanVertexBuffer(const void* data, size_t size) : m_baseBuffer(VulkanBaseBuffer(reinterpret_cast<const VulkanContext*>(m_context)))
	{
		Init(data, size);
	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		Cleanup();
	}

	void VulkanVertexBuffer::Init(const void* data, size_t size)
	{
		m_baseBuffer.CreateStaged(data, size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
		m_deleted = false;
	}

	void VulkanVertexBuffer::Cleanup()
	{
		const VulkanContext* vulkanContext = reinterpret_cast<const VulkanContext*>(m_context);
		CORE_ASSERT(vulkanContext, "Failed to cleanup VulkanVertexBuffer, vulkan context is null");

		if (!m_deleted)
		{
			m_baseBuffer.Free();
			m_deleted = true;
		}
	}

	void VulkanVertexBuffer::Free()
	{
		Cleanup();
	}

	void VulkanVertexBuffer::Bind() const
	{
		const VulkanContext* vulkanContext = reinterpret_cast<const VulkanContext*>(m_context);
		CORE_ASSERT(vulkanContext, "Failed to bind VulkanVertexBuffer, vulkan context is null");
		CORE_ASSERT(vulkanContext->CommandBuffers().size(), "Failed to bind VulkanVertexBuffer, vulkan command buffers are empty");

		for (size_t i = 0; i < vulkanContext->CommandBuffers().size(); i++)
		{
			VkBuffer vertexBuffers[] = { m_baseBuffer.Buffer() };
			VkDeviceSize offsets[] = { 0 }; //offset can be used to interleave vertex data
			vkCmdBindVertexBuffers(vulkanContext->CommandBuffers()[i], 0, 1, vertexBuffers, offsets);
		}
	}

	void VulkanVertexBuffer::Unbind() const
	{
	}

	VulkanIndexBuffer::VulkanIndexBuffer(const uint32_t* data, size_t size) : m_baseBuffer(VulkanBaseBuffer(reinterpret_cast<const VulkanContext*>(m_context)))
	{
		Init(data, size);
	}

	VulkanIndexBuffer::~VulkanIndexBuffer()
	{
		Cleanup();
	}

	void VulkanIndexBuffer::Init(const void* data, size_t size)
	{
		m_baseBuffer.CreateStaged(data, size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
		m_deleted = false;
	}

	void VulkanIndexBuffer::Cleanup()
	{
		const VulkanContext* vulkanContext = reinterpret_cast<const VulkanContext*>(m_context);
		CORE_ASSERT(vulkanContext, "Failed to cleanup VulkanIndexBuffer, vulkan context is null");

		if (!m_deleted)
		{
			m_baseBuffer.Free();
			m_deleted = true;
		}
	}

	void VulkanIndexBuffer::Free()
	{
		Cleanup();
	}

	void VulkanIndexBuffer::Bind() const
	{
		const VulkanContext* vulkanContext = reinterpret_cast<const VulkanContext*>(m_context);
		CORE_ASSERT(vulkanContext, "Failed to bind VulkanIndexBuffer, vulkan context is null");
		CORE_ASSERT(vulkanContext->CommandBuffers().size(), "Failed to bind VulkanIndexBuffer, vulkan command buffers are empty");

		for (size_t i = 0; i < vulkanContext->CommandBuffers().size(); i++)
		{
			VkDeviceSize offsets[] = { 0 }; //offset can be used to interleave vertex data
			vkCmdBindIndexBuffer(vulkanContext->CommandBuffers()[i], m_baseBuffer.Buffer(), 0, VK_INDEX_TYPE_UINT32);
		}
	}

	void VulkanIndexBuffer::Unbind() const
	{
	}
}