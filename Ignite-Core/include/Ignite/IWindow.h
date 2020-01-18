#pragma once
#include "NonCopyable.h"
#include "IRenderer.h"

namespace Ignite
{
	class IWindow : NonCopyable
	{
	protected:
		IWindow() = default;
		virtual void Init() = 0;
		virtual void Cleanup() = 0;
	public:
		virtual ~IWindow() {};

		static std::unique_ptr<IWindow> Create();
	protected:
		std::unique_ptr<IRenderer> m_renderer;
	};
}

