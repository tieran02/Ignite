#pragma once
#include "NonCopyable.h"

namespace Ignite
{
	class IWindow : NonCopyable
	{
	protected:
		IWindow(uint32_t width, uint32_t height);
		virtual void Init() = 0;
		virtual void Cleanup() = 0;
		uint32_t m_width, m_height;
	public:
		virtual ~IWindow() {};

		static IWindow& Create(uint32_t width, uint32_t height);
		static IWindow& GetInstance();

		virtual void PollEvents() = 0;
		virtual bool ShouldClose() const = 0;
		virtual void* GetHandle() const = 0;
		void Close();

		uint32_t Width() { return m_width; }
		uint32_t Height() { return m_height; }
		
	private:
		static std::unique_ptr<IWindow> s_instance;
	};
}

