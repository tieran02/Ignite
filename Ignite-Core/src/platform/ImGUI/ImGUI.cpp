#include "igpch.h"
#include "platform/ImGUI/ImGUI.h"

#include "Ignite/Log.h"
#include "Ignite/Renderer/IRendererAPI.h"
#include "Ignite/Renderer/Renderer.h"
#include "platform/ImGUI/VulkanImGUI.h"

std::unique_ptr<ImGUI> ImGUI::Create()
{
	CORE_ASSERT(Ignite::Renderer::IsInitialised(), "Failed to create buffer, Renderer is null");

	switch (Ignite::IRendererAPI::GetAPI())
	{
	case Ignite::IRendererAPI::API::NONE:    CORE_ASSERT(false, "IRendererAPI::NONE is currently not supported!"); return nullptr;
	case Ignite::IRendererAPI::API::VULKAN:	 return std::unique_ptr<VulkanImGUI>(new VulkanImGUI());
	}

	return nullptr;

}
