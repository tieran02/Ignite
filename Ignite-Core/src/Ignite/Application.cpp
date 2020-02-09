#include "igpch.h"
#include "Ignite/Application.h"
#include "Ignite/Log.h"
#include "Ignite/Events/ApplicationEvent.h"
#include "Ignite/Renderer/Renderer.h"
#include "Ignite/Renderer/RenderCommand.h"

namespace Ignite
{
	std::unique_ptr<Application> Application::s_instance = nullptr;
	
	Application::Application()
	{

	}

	Application& Application::Create()
	{	
		if (s_instance == nullptr)
		{
			LOG_CORE_INFO("Creating Application");
			s_instance = std::unique_ptr<Application>(new Application());
			s_instance->init();
		}
		else
		{
			LOG_CORE_WARNING("Application instance alredy exists, Only a single application is allowed");
		}
		return *s_instance;
	}

	Application& Application::Instance()
	{
		return *s_instance;
	}

	void Application::Reset()
	{
		s_instance.reset();
	}

	Application::~Application()
	{
		Close();
	}

	void Application::Start(uint32_t width, uint32_t height)
	{
		m_running = true;

		while (m_running)
		{
			m_window->OnUpdate();
			if(!m_running)
				break;
			
			for (Layer* layer : m_layerStack)
			{
				layer->OnUpdate();
			}
		}
	}

	void Application::Close()
	{
		if (!m_running)
			return;
		
		LOG_CORE_INFO("Ignite shutting down");

		m_running = false;
		
		//detach all layers
		for (Layer* layer : m_layerStack)
			layer->OnDetach();
		
		Renderer::Shutdown();
		m_window->Close();

		LOG_CORE_INFO("Ignite has been extinguished");
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(std::bind(&Application::OnWindowClose, this, std::placeholders::_1));

		dispatcher.Dispatch<WindowResizeEvent>(std::bind(&Application::OnWindowResize, this, std::placeholders::_1));


		//FL_LOG_CORE_TRACE(e.GetName());
		//handle layer events in reverse
		for (auto it = m_layerStack.end(); it != m_layerStack.begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		m_layerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_layerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::init()
	{
		//create window
		const WindowProperites window_properites;
		m_window = IWindow::Create(window_properites);
		m_window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));

		Renderer::Init();

	}

	bool Application::OnWindowClose(WindowCloseEvent e)
	{
		Close();
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent e)
	{
		if (e.GetWidth() > 0 && e.GetHeight() > 0) 
		{
			RenderCommand::SetViewport(0, 0, e.GetWidth(), e.GetHeight());
		}
		return true;
	}
}
