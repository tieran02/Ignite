#pragma once
#include "igpch.h"
#include "Ignite/IGraphicsContext.h"

#include <glm/glm.hpp>

namespace Ignite {
    class IRendererAPI : NonCopyable
	{
    public:
        enum class API {
            NONE,
            VULKAN
        };
	protected:
		IRendererAPI();
		virtual void Init() = 0;
		virtual void Cleanup() = 0;
    public:
		virtual ~IRendererAPI() {};

        virtual void BeginScene() = 0;
        virtual void EndScene() = 0;
    	
        virtual void SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

        virtual void SetClearColor(const glm::vec4& color) = 0;

        virtual void Clear() = 0;

        //virtual void DrawIndexed(const VertexArray& vertexArray) = 0;

        static API GetAPI() { return s_API; }

        static std::unique_ptr<IRendererAPI> Create();

        IGraphicsContext* GetGraphicsContext() const;

    private:
        static API s_API;
		std::unique_ptr<IGraphicsContext> m_graphicsContext;
    };
}