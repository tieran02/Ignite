#pragma once
#include "NonCopyable.h"

namespace Ignite
{
	class IPipeline;
	class IWindow;

	class IGraphicsContext : NonCopyable
    {
		friend class IPipeline;
	protected:
		IGraphicsContext() = default;
		virtual void Init() = 0;
		virtual void Cleanup() = 0;
    public:
		virtual ~IGraphicsContext() = default;
        virtual void SwapBuffers() = 0;

        static std::unique_ptr<IGraphicsContext> Create();
	protected:
		std::unordered_map<std::string,std::shared_ptr<IPipeline>> m_pipelines;
    };
}