#pragma once
#include "RendererAPI.h"

namespace Ignite {
	class RenderCommand
	{
	public:
		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

		static void SetClearColor(const glm::vec4& color);

		static void Clear();

		static void DrawIndexed(const ::Ignite::Buffer* vertexBuffer, const ::Ignite::Buffer* indexBuffer, uint32_t indexCount, const glm::mat4& transform);
	};
}
