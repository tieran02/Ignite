#include "igpch.h"
#include "platform/Vulkan/VulkanBuffer.h"
#include "vulkan/vulkan.h"
#include "platform/Vulkan/VulkanContext.h"
#include "Ignite/Log.h"
#include "platform/Vulkan/VulkanResources.h"

namespace Ignite
{

	void VulkanBuffer::Create(const void* data, VkDeviceSize size, VkBufferUsageFlags usage, BUFFER_VISIBILITY visibility)
	{
		m_size = size;
		
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

	void VulkanBuffer::CreateStaged(const void* data, VkDeviceSize size, VkBufferUsageFlags usage)
	{
		//staging buffer
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		
		VulkanResources::CreateBuffer(m_vulkanContext.Device().LogicalDevice(), m_vulkanContext.Device().PhysicalDevice(),size,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer, stagingBufferMemory);

		void* bufferData = nullptr;
		vkMapMemory(m_vulkanContext.Device().LogicalDevice(), stagingBufferMemory, 0, size, 0, &bufferData);
		memcpy(bufferData, data, (size_t)size);
		vkUnmapMemory(m_vulkanContext.Device().LogicalDevice(), stagingBufferMemory);

		//data buffer
		VulkanResources::CreateBuffer(m_vulkanContext.Device().LogicalDevice(), m_vulkanContext.Device().PhysicalDevice(), size,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			m_vkBuffer, m_vkBufferMemory);
		

		VulkanResources::CopyBuffer(m_vulkanContext.Device().LogicalDevice(), m_vulkanContext.CommandPool(), m_vulkanContext.Device().GraphicsQueue(),
			stagingBuffer, m_vkBuffer, size);

		vkDestroyBuffer(m_vulkanContext.Device().LogicalDevice(), stagingBuffer, nullptr);
		vkFreeMemory(m_vulkanContext.Device().LogicalDevice(), stagingBufferMemory, nullptr);
	}

	void VulkanBuffer::CreateHostVisable(const void* data, VkDeviceSize size, VkBufferUsageFlags usage)
	{
		VulkanResources::CreateBuffer(m_vulkanContext.Device().LogicalDevice(), m_vulkanContext.Device().PhysicalDevice(), size,
			usage, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			m_vkBuffer, m_vkBufferMemory);
	}



	VulkanBuffer::VulkanBuffer(const BufferCreateInfo& bufferInfo, const VulkanContext& context) : Buffer(bufferInfo), m_vulkanContext(context)
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
			Create(m_bufferInfo.GetData(), m_bufferInfo.GetSize(), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, m_bufferInfo.GetBufferVisibility());
			break;
		case BUFFER_TYPE::VERTEX:
			Create(m_bufferInfo.GetData(), m_bufferInfo.GetSize(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, m_bufferInfo.GetBufferVisibility());
			break;
		case BUFFER_TYPE::INDEX:
			Create(m_bufferInfo.GetData(), m_bufferInfo.GetSize(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, m_bufferInfo.GetBufferVisibility());
			break;
		case BUFFER_TYPE::UNIFORM:
			Create(m_bufferInfo.GetData(), m_bufferInfo.GetSize(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, m_bufferInfo.GetBufferVisibility());
			break;
		case BUFFER_TYPE::STORAGE:
			Create(m_bufferInfo.GetData(), m_bufferInfo.GetSize(), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, m_bufferInfo.GetBufferVisibility());
			break;
		case BUFFER_TYPE::TRANSFER:
			Create(m_bufferInfo.GetData(), m_bufferInfo.GetSize(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, m_bufferInfo.GetBufferVisibility());
			break;
		default: ;
		}
		m_deleted = false;
	}

	void VulkanBuffer::Cleanup()
	{
		if (m_vkBuffer != nullptr)
		{
			vkDestroyBuffer(m_vulkanContext.Device().LogicalDevice(), m_vkBuffer, nullptr);
			m_vkBuffer = nullptr;
		}
		if (m_vkBufferMemory != nullptr)
		{
			vkFreeMemory(m_vulkanContext.Device().LogicalDevice(), m_vkBufferMemory, nullptr);
			m_vkBufferMemory = nullptr;
		}
	}

	void VulkanBuffer::Free()
	{
		Cleanup();
	}

	void VulkanBuffer::Bind() const
	{
		CORE_ASSERT(m_vulkanContext.CommandBuffers().size(), "Failed to bind VulkanVertexBuffer, vulkan command buffers are empty");
		
		switch (m_bufferInfo.GetBufferType())
		{
		case BUFFER_TYPE::BUFFER:
			CORE_ASSERT(false, "Bind for BUFFER_TYPE::BUFFER is not currently supported");
			break;
		case BUFFER_TYPE::VERTEX:
			for (size_t i = 0; i < m_vulkanContext.CommandBuffers().size(); i++)
			{
				VkBuffer vertexBuffers[] = { m_vkBuffer };
				VkDeviceSize offsets[] = { 0 }; //offset can be used to interleave vertex data
				vkCmdBindVertexBuffers(m_vulkanContext.CommandBuffers()[i], 0, 1, vertexBuffers, offsets);
			}
			break;
		case BUFFER_TYPE::INDEX:
			for (size_t i = 0; i < m_vulkanContext.CommandBuffers().size(); i++)
			{
				VkDeviceSize offsets[] = { 0 }; //offset can be used to interleave vertex data
				vkCmdBindIndexBuffer(m_vulkanContext.CommandBuffers()[i], m_vkBuffer, 0, VK_INDEX_TYPE_UINT32);
			}
			break;
		default:;
		}
	}

	void VulkanBuffer::Unbind() const
	{
		//TODO IMPLEMENT UNBIND BUFFER
	}

	void* VulkanBuffer::Map() const
	{
		if(m_bufferInfo.GetBufferVisibility() == BUFFER_VISIBILITY::HOST)
		{
			void* data;
			vkMapMemory(m_vulkanContext.Device().LogicalDevice(), m_vkBufferMemory, 0, m_size, 0, &data);
			return data;
		}
		else
		{
			LOG_CORE_ERROR("Can't map to a buffer that is not host visible");
			return nullptr;
		}
	}

	void VulkanBuffer::Unmap() const
	{
		if (m_bufferInfo.GetBufferVisibility() == BUFFER_VISIBILITY::HOST )
		{
			vkUnmapMemory(m_vulkanContext.Device().LogicalDevice(), m_vkBufferMemory);
		}
		else
		{
			LOG_CORE_ERROR("Can't map to a buffer that is not host visible");
		}
	}

	void VulkanBuffer::Flush() const
	{
		if (m_bufferInfo.GetBufferVisibility() == BUFFER_VISIBILITY::HOST)
		{
			VkMappedMemoryRange vertexMappedRange = {};
			vertexMappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
			vertexMappedRange.memory = m_vkBufferMemory;
			vertexMappedRange.offset = 0;
			vertexMappedRange.size = VK_WHOLE_SIZE;
			vkFlushMappedMemoryRanges(m_vulkanContext.Device().LogicalDevice(), 1, &vertexMappedRange);
		}
		else
		{
			LOG_CORE_ERROR("Can't map to a buffer that is not host visible");
		}
	}

	void VulkanBuffer::CopyToBuffer(void* data, size_t size)
	{
		if (m_bufferInfo.GetBufferVisibility() == BUFFER_VISIBILITY::HOST)
		{
			void* dst_data;
			vkMapMemory(m_vulkanContext.Device().LogicalDevice(), m_vkBufferMemory, 0, m_size, 0, &dst_data);
			memcpy(dst_data, data, size);
			vkUnmapMemory(m_vulkanContext.Device().LogicalDevice(), m_vkBufferMemory);
		}
		else
		{
			LOG_CORE_ERROR("Can't copy to a buffer that is not host visible");
		}
	}
}
