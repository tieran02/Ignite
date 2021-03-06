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

		static void DrawIndexed(const ::Ignite::IBuffer* vertexBuffer, const ::Ignite::IBuffer* indexBuffer, uint32_t indexCount, const glm::mat4& transform);
	private:
		static std::unique_ptr<IRendererAPI> s_renderer;
	};
}
