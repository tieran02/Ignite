#include "igpch.h"
#include <Ignite/Log.h>
#include "platform/Vulkan/VulkanRenderer.h"
#include "platform/Vulkan/VulkanContext.h"

Ignite::VulkanRenderer::VulkanRenderer() : Renderer()
{
	Init();
}

void Ignite::VulkanRenderer::Init()
{
    LOG_CORE_INFO("Initialising VulkanRenderer");
}

void Ignite::VulkanRenderer::Cleanup()
{
	LOG_CORE_INFO("Cleaning VulkanRenderer");
}

Ignite::VulkanRenderer::~VulkanRenderer()
{
	Cleanup();
}

void Ignite::VulkanRenderer::SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {

}

void Ignite::VulkanRenderer::SetClearColor(const glm::vec4 &color) {

}

void Ignite::VulkanRenderer::Clear() {

}
