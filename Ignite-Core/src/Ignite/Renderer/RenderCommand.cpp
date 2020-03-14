#include "igpch.h"
#include "Ignite/Renderer/RenderCommand.h"
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
		s_renderer->SetViewPort(x, y, width, height);
	}

	void RenderCommand::SetClearColor(const glm::vec4& color)
	{
		s_renderer->SetClearColor(color);
	}

	void RenderCommand::Clear()
	{
	}

	void RenderCommand::DrawIndexed(const IVertexBuffer* vertexBuffer, const IIndexBuffer* indexBuffer, uint32_t indexCount)
	{
		s_renderer->DrawIndexed(vertexBuffer,indexBuffer,indexCount);
	}

	void RenderCommand::SetSceneUniformBuffer(const SceneUniformBuffer& ubo)
	{
		s_renderer->SetSceneUniformBufferObject(ubo);
	}
}
