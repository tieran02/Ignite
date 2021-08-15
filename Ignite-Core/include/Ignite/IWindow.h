#pragma once
#include "NonCopyable.h"
#include "Ignite/CreateProperties.h"
#include "Events/Event.h"
#include <functional>

namespace Ignite
{
	using EventCallbackFn = std::function<void(Event&)>;

	struct WindowProperites : public CreateProperties
	{
		std::string Title;
		unsigned int Width;
		unsigned int Height;
		bool VSync;

		EventCallbackFn EventCallback;

		WindowProperites(const std::string& title = "Ignite Engine", unsigned int width = 1280, unsigned int height = 720)
			: Title(title), Width(width), Height(height), VSync(false), CreateProperties(CreatePropertyType::Window) {}
	};
	
	class IWindow : NonCopyable
	{
	protected:
		IWindow(const WindowProperites& properites);
		virtual void Init() = 0;
		virtual void Cleanup() = 0;

		WindowProperites m_properites;
	public:
		virtual ~IWindow() {};

		static std::unique_ptr<IWindow> Create(const WindowProperites& properites);

		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void OnUpdate() = 0;
		virtual void* GetHandle() const = 0;
		virtual void Close() = 0;

		uint32_t Width() const { return m_properites.Width; }
		uint32_t Height() const { return m_properites.Height; }
	};
}

