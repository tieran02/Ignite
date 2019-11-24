//
// Created by Tieran on 24/11/2019.
//

#include <ignite/Log.h>
#include "platform/vulkan/VulkanRenderAPI.h"

Ignite::VulkanRenderAPI::VulkanRenderAPI()
{
    LOG_CORE_INFO("Using VulkanRenderAPI");
    Init();
}

Ignite::VulkanRenderAPI::~VulkanRenderAPI()
{

}

void Ignite::VulkanRenderAPI::Init() {
    LOG_CORE_INFO("Initialising VulkanRenderAPI");
}

void Ignite::VulkanRenderAPI::SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {

}

void Ignite::VulkanRenderAPI::SetClearColor(const glm::vec4 &color) {

}

void Ignite::VulkanRenderAPI::Clear() {

}
