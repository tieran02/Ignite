#include "igpch.h"
#include "Ignite/Renderer/RenderCommand.h"
#include "Ignite/Log.h"
#include "glm/gtx/associated_min_max.hpp"
#include "glm/gtx/associated_min_max.hpp"
#include "Ignite/Application.h"

namespace Ignite
{

	void RenderCommand::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		Application::Instance().GetRenderer()->SetViewPort(x, y, width, height);
	}

	void RenderCommand::SetClearColor(const glm::vec4& color)
	{
		Application::Instance().GetRenderer()->SetClearColor(color);
	}

	void RenderCommand::Clear()
	{
	}

	void RenderCommand::DrawIndexed(const Ref<Buffer>& vertexBuffer, const Ref<Buffer>& indexBuffer, uint32_t indexCount, const glm::mat4& transform)
	{
		Application::Instance().GetRenderer()->DrawIndexed(vertexBuffer,indexBuffer,indexCount, transform);
	}
}
