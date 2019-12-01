#pragma once
#include "igpch.h"
#include <glm/glm.hpp>
//#include "GraphicsContext.h"

namespace Ignite {
    class Renderer {
    public:
        enum class API {
            NONE,
            VULKAN
        };
    public:
		virtual ~Renderer() = default;

        virtual void Init() = 0;

        virtual void SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

        virtual void SetClearColor(const glm::vec4& color) = 0;

        virtual void Clear() = 0;

        //virtual void DrawIndexed(const VertexArray& vertexArray) = 0;

        static API GetAPI() { return s_API; }

        static std::unique_ptr<Renderer> Create();

    private:
        static API s_API;
        //std::unique_ptr<GraphicsContext> m_graphicsContext;
    };
}
