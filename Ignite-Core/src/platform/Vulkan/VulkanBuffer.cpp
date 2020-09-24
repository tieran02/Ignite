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

	void VulkanBaseBuffer::Create(const void* data, VkDeviceSize size, VkBufferUsageFlags usage, BUFFER_VISIBILITY visibility)
	{
		switch (visibility)
		{
		case BUFFER_VISIBILITY::HOST:
			CreateHostVisable(data, size, usage);
			break;
		case BUFFER_VISIBILITY::STAGED:
			CreateStaged(data, size, usage);
			break;
		default: 
			CORE_ASSERT(false, "INVALID BUFFER_VISIBILITY for Vulkan buffer");
		}
	}

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

	VulkanBuffer::VulkanBuffer(const BufferCreateInfo& bufferInfo) : IBuffer(bufferInfo), m_baseBuffer(VulkanBaseBuffer(reinterpret_cast<const VulkanContext*>(m_context)))
	{
		Init();
	}

	VulkanBuffer::~VulkanBuffer()
	{
		Cleanup();
	}

	void VulkanBuffer::Init()
	{
		switch (m_bufferInfo.GetBufferType())
		{
		case BUFFER_TYPE::BUFFER:
			m_baseBuffer.Create(m_bufferInfo.GetData(), m_bufferInfo.GetSize(), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, m_bufferInfo.GetBufferVisibility());
			break;
		case BUFFER_TYPE::VERTEX:
			m_baseBuffer.Create(m_bufferInfo.GetData(), m_bufferInfo.GetSize(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, m_bufferInfo.GetBufferVisibility());
			break;
		case BUFFER_TYPE::INDEX:
			m_baseBuffer.Create(m_bufferInfo.GetData(), m_bufferInfo.GetSize(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, m_bufferInfo.GetBufferVisibility());
			break;
		default: ;
		}
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
		const VulkanContext* vulkanContext = reinterpret_cast<const VulkanContext*>(m_context);
		CORE_ASSERT(vulkanContext, "Failed to bind VulkanVertexBuffer, vulkan context is null");
		CORE_ASSERT(vulkanContext->CommandBuffers().size(), "Failed to bind VulkanVertexBuffer, vulkan command buffers are empty");
		
		switch (m_bufferInfo.GetBufferType())
		{
		case BUFFER_TYPE::BUFFER:
			CORE_ASSERT(false, "Bind for BUFFER_TYPE::BUFFER is not currently supported");
			break;
		case BUFFER_TYPE::VERTEX:
			for (size_t i = 0; i < vulkanContext->CommandBuffers().size(); i++)
			{
				VkBuffer vertexBuffers[] = { m_baseBuffer.Buffer() };
				VkDeviceSize offsets[] = { 0 }; //offset can be used to interleave vertex data
				vkCmdBindVertexBuffers(vulkanContext->CommandBuffers()[i], 0, 1, vertexBuffers, offsets);
			}
			break;
		case BUFFER_TYPE::INDEX:
			for (size_t i = 0; i < vulkanContext->CommandBuffers().size(); i++)
			{
				VkDeviceSize offsets[] = { 0 }; //offset can be used to interleave vertex data
				vkCmdBindIndexBuffer(vulkanContext->CommandBuffers()[i], m_baseBuffer.Buffer(), 0, VK_INDEX_TYPE_UINT32);
			}
			break;
		default:;
		}
	}

	void VulkanBuffer::Unbind() const
	{
		//TODO IMPLEMENT UNBIND BUFFER
	}

	void VulkanBuffer::Map() const
	{
	}

	void VulkanBuffer::Unmap() const
	{
	}

	void VulkanBuffer::Flush() const
	{
	}
}
