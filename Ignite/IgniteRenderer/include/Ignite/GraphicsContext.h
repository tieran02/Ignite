#pragma once

namespace Ignite
{
    class GraphicsContext : NonCopyable
    {
	protected:
		GraphicsContext() = default;
		virtual void Init() = 0;
		virtual void Cleanup() = 0;
    public:
		virtual ~GraphicsContext() = default;
        virtual void SwapBuffers() = 0;

        static std::unique_ptr<GraphicsContext> Create();
    };
}