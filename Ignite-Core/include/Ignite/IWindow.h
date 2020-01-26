#pragma once
#include "NonCopyable.h"
#include "IRendererAPI.h"

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

		static IWindow& Create();
		static IWindow& GetInstance();
		
		virtual void* GetHandle() const = 0;
		void Close();
	private:
		static std::unique_ptr<IWindow> s_instance;
	};
}

