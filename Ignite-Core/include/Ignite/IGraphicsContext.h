#pragma once
#include "NonCopyable.h"

namespace Ignite
{
	class IWindow;

	class IGraphicsContext : NonCopyable
    {
	protected:
		IGraphicsContext() = default;
		virtual void Init() = 0;
		virtual void Cleanup() = 0;
    public:
		virtual ~IGraphicsContext() = default;
        virtual void SwapBuffers() const = 0;

        static std::unique_ptr<IGraphicsContext> Create();
    };
}