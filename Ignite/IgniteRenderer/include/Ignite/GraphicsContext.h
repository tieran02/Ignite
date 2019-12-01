#pragma once

namespace Ignite
{
    class GraphicsContext
    {
    public:
        virtual ~GraphicsContext() = 0;
        virtual void Init() = 0;
        virtual void SwapBuffers() = 0;

        static std::unique_ptr<GraphicsContext> Create();
    };
}