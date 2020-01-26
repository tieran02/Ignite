#include "igpch.h"
#include <Ignite/Log.h>
#include "platform/Vulkan/VulkanRendererAPI.h"
#include "platform/Vulkan/VulkanContext.h"

Ignite::VulkanRendererAPI::VulkanRendererAPI()
{
	Init();
}

void Ignite::VulkanRendererAPI::Init()
{
    LOG_CORE_INFO("Initialising VulkanRendererAPI");
}

void Ignite::VulkanRendererAPI::Cleanup()
{
	LOG_CORE_INFO("Cleaning VulkanRendererAPI");
}

Ignite::VulkanRendererAPI::~VulkanRendererAPI()
{
	Cleanup();
}

void Ignite::VulkanRendererAPI::SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {

}

void Ignite::VulkanRendererAPI::SetClearColor(const glm::vec4 &color) {

}

void Ignite::VulkanRendererAPI::Clear() {

}
