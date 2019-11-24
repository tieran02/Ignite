#ifndef IGNITERENDERER_RENDERERAPI_H
#define IGNITERENDERER_RENDERERAPI_H

#include <cstdint>
#include <memory>
#include <glm/glm.hpp>

namespace Ignite {
    class RendererAPI {
    public:
        enum class API {
            NONE,
            VULKAN
        };
    public:
        virtual void Init() = 0;

        virtual void SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

        virtual void SetClearColor(const glm::vec4& color) = 0;

        virtual void Clear() = 0;

        //virtual void DrawIndexed(const VertexArray& vertexArray) = 0;

        static API GetAPI() { return s_API; }

        static std::unique_ptr<RendererAPI> Create();

    private:
        static API s_API;
    };
}
#endif //IGNITERENDERER_RENDERERAPI_H