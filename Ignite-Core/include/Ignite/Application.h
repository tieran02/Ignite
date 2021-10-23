#pragma once
#include "IWindow.h"
#include "Layer.h"
#include <mutex>

namespace spdlog
{
	class logger;
}

namespace Ignite
{
	class WindowCloseEvent;
	class WindowResizeEvent;
	class Layer;
	class Event;
	class RendererAPI;

	class Application
	{
	protected:
		Application();
	public:
		static Application& Create();
		static Application& Instance();
		static void Reset();
		
		virtual ~Application();

		void Start(uint32_t width, uint32_t height);
		void Close();
		
		void OnEvent(Event& e);
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		const IWindow* Window() const { return m_window.get(); }
		RendererAPI* GetRenderer() const { return m_renderer.get(); }
	private:
		void init();
		
		bool OnWindowClose(WindowCloseEvent e);
		bool OnWindowResize(WindowResizeEvent e);
		
		static std::unique_ptr<Application> s_instance;
		
		std::unique_ptr<IWindow> m_window;
		std::unique_ptr<RendererAPI> m_renderer;
		bool m_running{ true };
		LayerStack m_layerStack;
	};
}
