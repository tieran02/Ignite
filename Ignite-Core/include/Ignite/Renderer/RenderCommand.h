#pragma once
#include "IRendererAPI.h"

namespace Ignite {
	class RenderCommand
	{
		friend class Renderer;
	public:
		static void Init();

		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

		static void SetClearColor(const glm::vec4& color);

		static void Clear();

		static void DrawIndexed(const ::Ignite::IVertexBuffer* vertexBuffer, const ::Ignite::IIndexBuffer* indexBuffer, uint32_t indexCount, const glm::mat4& transform);

		static void SetSceneUniformBuffer(const SceneUniformBuffer& ubo);
	private:
		static std::unique_ptr<IRendererAPI> s_renderer;
	};
}
