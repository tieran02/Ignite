#include "igpch.h"
#include "Ignite/Ignite.h"
#include "Ignite/Log.h"
#include "Ignite/IWindow.h"
#include "Ignite/Renderer.h"

namespace Ignite
{
	Ignite::Ignite() : m_running(false)
	{
		
	}

	Ignite::~Ignite()
	{
		Close();
	}

	void Ignite::Init()
	{
		Log::Init();

		LOG_CORE_INFO("Ignition has started");
		
		IWindow::Create();
		Renderer::Init();
		m_running = true;
	}

	void Ignite::Close()
	{
		if (!m_running)
			return;

		LOG_CORE_INFO("Ignite shutting down");
		
		Renderer::Shutdown();
		IWindow::GetInstance().Close();
		m_running = false;

		LOG_CORE_INFO("Ignite has been extinguished");
	}
}
