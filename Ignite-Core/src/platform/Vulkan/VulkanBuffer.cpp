#include "igpch.h"
#include "platform/Vulkan/VulkanBuffer.h"
#include "vulkan/vulkan.h"
#include "platform/Vulkan/VulkanContext.h"
#include "Ignite/Log.h"

namespace Ignite
{
	VulkanBaseBuffer::VulkanBaseBuffer(const VulkanContext* context) : m_context(context)
	{}

	void VulkanBaseBuffer::Create(void* data, VkDeviceSize size, VkBufferUsageFlags usage)
	{
		//staging buffer
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		createBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* bufferData = nullptr;
		vkMapMemory(m_context->Device().LogicalDevice(), stagingBufferMemory, 0, size, 0, &bufferData);
		memcpy(bufferData, data, size);
		vkUnmapMemory(m_context->Device().LogicalDevice(), stagingBufferMemory);

		//data buffer
		createBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_vkBuffer, m_vkBufferMemory);

		copyBuffer(stagingBuffer, m_vkBuffer, size);

		vkDestroyBuffer(m_context->Device().LogicalDevice(), stagingBuffer, nullptr);
		vkFreeMemory(m_context->Device().LogicalDevice(), stagingBufferMemory, nullptr);
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

	void VulkanBaseBuffer::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
		VkBuffer& buffer, VkDeviceMemory& bufferMemory)
	{		
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VK_CHECK_RESULT(vkCreateBuffer(m_context->Device().LogicalDevice(), &bufferInfo, nullptr, &buffer));

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(m_context->Device().LogicalDevice(), buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

		VK_CHECK_RESULT(vkAllocateMemory(m_context->Device().LogicalDevice(), &allocInfo, nullptr, &bufferMemory));

		vkBindBufferMemory(m_context->Device().LogicalDevice(), buffer, bufferMemory, 0);
	}

	void VulkanBaseBuffer::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
	{
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = m_context->CommandPool();
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(m_context->Device().LogicalDevice(), &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		VkBufferCopy copyRegion = {};
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(m_context->Device().GraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(m_context->Device().GraphicsQueue());

		vkFreeCommandBuffers(m_context->Device().LogicalDevice(), m_context->CommandPool(), 1, &commandBuffer);
	}

	uint32_t VulkanBaseBuffer::findMemoryType(uint32_t typeFilter,
		VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(m_context->Device().PhysicalDevice(), &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}

		LOG_CORE_FATAL("failed to find suitable memory type!");
	}

	VulkanBuffer::VulkanBuffer(void* data, size_t size) : m_baseBuffer(VulkanBaseBuffer(reinterpret_cast<const VulkanContext*>(m_context)))
	{
		Init(data,size);
	}

	VulkanBuffer::~VulkanBuffer()
	{
		Cleanup();
	}

	void VulkanBuffer::Init(void* data, size_t size)
	{
		m_baseBuffer.Create(data, size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
		m_deleted = false;
	}

	void VulkanBuffer::Cleanup()
	{
		const VulkanContext* vulkanContext = reinterpret_cast<const VulkanContext*>(m_context);
		CORE_ASSERT(vulkanContext, "Failed to cleanup VulkanBuffer, vulkan context is null");
		
		if(!m_deleted)
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


	
	VulkanVertexBuffer::VulkanVertexBuffer(float* data, size_t size) : m_baseBuffer(VulkanBaseBuffer(reinterpret_cast<const VulkanContext*>(m_context)))
	{
		Init(data, size);
	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		Cleanup();
	}

	void VulkanVertexBuffer::Init(void* data, size_t size)
	{
		m_baseBuffer.Create(data, size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
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

	VulkanIndexBuffer::VulkanIndexBuffer(uint16_t* data, size_t size) : m_baseBuffer(VulkanBaseBuffer(reinterpret_cast<const VulkanContext*>(m_context)))
	{
		Init(data, size);
	}

	VulkanIndexBuffer::~VulkanIndexBuffer()
	{
		Cleanup();
	}

	void VulkanIndexBuffer::Init(void* data, size_t size)
	{
		m_baseBuffer.Create(data, size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
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
		CORE_ASSERT(vulkanContext, "Failed to bind VulkanVertexBuffer, vulkan context is null");
		CORE_ASSERT(vulkanContext->CommandBuffers().size(), "Failed to bind VulkanVertexBuffer, vulkan command buffers are empty");

		for (size_t i = 0; i < vulkanContext->CommandBuffers().size(); i++)
		{
			VkDeviceSize offsets[] = { 0 }; //offset can be used to interleave vertex data
			vkCmdBindIndexBuffer(vulkanContext->CommandBuffers()[i], m_baseBuffer.Buffer(), 0, VK_INDEX_TYPE_UINT16);
		}
	}

	void VulkanIndexBuffer::Unbind() const
	{
	}
}
