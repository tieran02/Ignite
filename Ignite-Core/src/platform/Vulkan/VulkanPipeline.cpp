#include "igpch.h"
#include "platform/Vulkan/VulkanPipeline.h"
#include "platform/Vulkan/VulkanContext.h"
#include "Ignite/Log.h"

namespace  Ignite
{
	VulkanPipeline::VulkanPipeline(const std::string& vertexShader, const std::string& fragmentShader)
	: IPipeline(vertexShader, fragmentShader)
	{
		Init();
	}

	VulkanPipeline::~VulkanPipeline()
	{
		Cleanup();
	}

	void VulkanPipeline::Init()
	{
		LOG_CORE_INFO("Creating Vulkan Pipeline");
		//const VulkanContext* vulkanContext = reinterpret_cast<const VulkanContext*>(m_context);
		//auto device = vulkanContext->Device().LogicalDevice();
	}

	void VulkanPipeline::Cleanup()
	{
		LOG_CORE_INFO("Cleaning up Vulkan Pipeline");
	}

	void VulkanPipeline::Bind() const
	{
	}

	void VulkanPipeline::Unbind() const
	{
	}
}
