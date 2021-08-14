#include "igpch.h"
#include "Ignite/Renderer/Buffer.h"
#include "Ignite/Log.h"
#include "Ignite/Renderer/Renderer.h"
#include "Ignite/Renderer/RendererAPI.h"
#include "platform/Vulkan/VulkanBuffer.h"
#include "platform/Vulkan/VulkanContext.h"

Ignite::Buffer::Buffer(const BufferCreateInfo& bufferInfo) : m_bufferInfo(bufferInfo)
{
}

std::unique_ptr<Ignite::Buffer> Ignite::Buffer::Create(const BufferCreateInfo& bufferInfo)
{
	//CORE_ASSERT(Renderer::IsInitialised(), "Failed to create buffer, Renderer is null")

		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::NONE:    CORE_ASSERT(false, "IRendererAPI::NONE is currently not supported!"); return nullptr;
		case RendererAPI::API::VULKAN:
			return std::unique_ptr<VulkanBuffer>(new VulkanBuffer(bufferInfo, *reinterpret_cast<VulkanContext*>(Renderer::GraphicsContext())));
		}

	return nullptr;
}

