#include "igpch.h"
#include "Ignite/RenderCommand.h"
#include "Ignite/Log.h"

namespace Ignite
{
	std::unique_ptr<IRendererAPI> RenderCommand::s_renderer = nullptr;
	
	void RenderCommand::Init()
	{
		s_renderer = IRendererAPI::Create();
		CORE_ASSERT(s_renderer, "Renderer is null");
	}

	void RenderCommand::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
	}

	void RenderCommand::SetClearColor(const glm::vec4& color)
	{
		s_renderer->SetClearColor(color);
	}

	void RenderCommand::Clear()
	{
	}

	void RenderCommand::DrawIndexed()
	{
	}
}
