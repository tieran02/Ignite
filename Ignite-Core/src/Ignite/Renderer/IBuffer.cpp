#include "igpch.h"
#include "Ignite/Renderer/IBuffer.h"
#include "Ignite/Log.h"
#include "Ignite/Renderer/Renderer.h"
#include "Ignite/Renderer/IRendererAPI.h"
#include "platform/Vulkan/VulkanBuffer.h"
#include "platform/Vulkan/VulkanContext.h"

Ignite::IBuffer::IBuffer() : m_context(Renderer::GraphicsContext())
{
}

std::shared_ptr<Ignite::IBuffer> Ignite::IBuffer::Create(void* data, size_t size)
{
	CORE_ASSERT(Renderer::IsInitialised(), "Failed to create buffer, Renderer is null")

		switch (IRendererAPI::GetAPI())
		{
		case IRendererAPI::API::NONE:    CORE_ASSERT(false, "IRendererAPI::NONE is currently not supported!"); return nullptr;
		case IRendererAPI::API::VULKAN:
			std::shared_ptr<IBuffer> buffer = std::shared_ptr<VulkanBuffer>(new VulkanBuffer(data,size));
			Renderer::GraphicsContext()->m_buffers.push_back(buffer);
			return buffer;
		}

	return nullptr;
}

Ignite::IVertexBuffer::IVertexBuffer() : IBuffer()
{
	
}

std::shared_ptr<Ignite::IVertexBuffer> Ignite::IVertexBuffer::Create(float* data, size_t size)
{
	CORE_ASSERT(Renderer::IsInitialised(), "Failed to create vertex buffer, Renderer is null")

		switch (IRendererAPI::GetAPI())
		{
		case IRendererAPI::API::NONE:    CORE_ASSERT(false, "IRendererAPI::NONE is currently not supported!"); return nullptr;
		case IRendererAPI::API::VULKAN:
			std::shared_ptr<IVertexBuffer> buffer = std::shared_ptr<VulkanVertexBuffer>(new VulkanVertexBuffer(data,size));
			Renderer::GraphicsContext()->m_buffers.push_back(std::static_pointer_cast<IBuffer>(buffer));
			return buffer;
		}

	return nullptr;
}

Ignite::IIndexBuffer::IIndexBuffer() : IBuffer()
{
}

std::shared_ptr<Ignite::IIndexBuffer> Ignite::IIndexBuffer::Create(uint16_t* data, size_t size)
{
	CORE_ASSERT(Renderer::IsInitialised(), "Failed to create vertex buffer, Renderer is null")

		switch (IRendererAPI::GetAPI())
		{
		case IRendererAPI::API::NONE:    CORE_ASSERT(false, "IRendererAPI::NONE is currently not supported!"); return nullptr;
		case IRendererAPI::API::VULKAN:
			std::shared_ptr<IIndexBuffer> buffer = std::shared_ptr<IIndexBuffer>(new VulkanIndexBuffer(data, size));
			Renderer::GraphicsContext()->m_buffers.push_back(std::static_pointer_cast<IBuffer>(buffer));
			return buffer;
		}

	return nullptr;
}

