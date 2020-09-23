#include "igpch.h"
#include "Ignite/Renderer/IBuffer.h"
#include "Ignite/Log.h"
#include "Ignite/Renderer/Renderer.h"
#include "Ignite/Renderer/IRendererAPI.h"
#include "platform/Vulkan/VulkanBuffer.h"
#include "platform/Vulkan/VulkanContext.h"

Ignite::IBuffer::IBuffer(const BufferCreateInfo& bufferInfo) : m_context(Renderer::GraphicsContext()), m_bufferInfo(bufferInfo)
{
}

std::shared_ptr<Ignite::IBuffer> Ignite::IBuffer::Create(const BufferCreateInfo& bufferInfo)
{
	CORE_ASSERT(Renderer::IsInitialised(), "Failed to create buffer, Renderer is null")

		switch (IRendererAPI::GetAPI())
		{
		case IRendererAPI::API::NONE:    CORE_ASSERT(false, "IRendererAPI::NONE is currently not supported!"); return nullptr;
		case IRendererAPI::API::VULKAN:
			std::shared_ptr<IBuffer> buffer = std::shared_ptr<VulkanBuffer>(new VulkanBuffer(bufferInfo));
			Renderer::GraphicsContext()->m_buffers.push_back(buffer);
			return buffer;
		}

	return nullptr;
}

Ignite::IVertexBuffer::IVertexBuffer(const BufferCreateInfo& bufferInfo) : IBuffer(bufferInfo)
{
	
}

std::shared_ptr<Ignite::IVertexBuffer> Ignite::IVertexBuffer::Create(const BufferCreateInfo& bufferInfo)
{
	CORE_ASSERT(Renderer::IsInitialised(), "Failed to create vertex buffer, Renderer is null")

		switch (IRendererAPI::GetAPI())
		{
		case IRendererAPI::API::NONE:    CORE_ASSERT(false, "IRendererAPI::NONE is currently not supported!"); return nullptr;
		case IRendererAPI::API::VULKAN:
			std::shared_ptr<IVertexBuffer> buffer = std::shared_ptr<VulkanVertexBuffer>(new VulkanVertexBuffer(bufferInfo));
			Renderer::GraphicsContext()->m_buffers.push_back(std::static_pointer_cast<IBuffer>(buffer));
			return buffer;
		}

	return nullptr;
}

Ignite::IIndexBuffer::IIndexBuffer(const BufferCreateInfo& bufferInfo) : IBuffer(bufferInfo)
{
}

std::shared_ptr<Ignite::IIndexBuffer> Ignite::IIndexBuffer::Create(const BufferCreateInfo& bufferInfo)
{
	CORE_ASSERT(Renderer::IsInitialised(), "Failed to create vertex buffer, Renderer is null")

		switch (IRendererAPI::GetAPI())
		{
		case IRendererAPI::API::NONE:    CORE_ASSERT(false, "IRendererAPI::NONE is currently not supported!"); return nullptr;
		case IRendererAPI::API::VULKAN:
			std::shared_ptr<IIndexBuffer> buffer = std::shared_ptr<IIndexBuffer>(new VulkanIndexBuffer(bufferInfo));
			Renderer::GraphicsContext()->m_buffers.push_back(std::static_pointer_cast<IBuffer>(buffer));
			return buffer;
		}

	return nullptr;
}

