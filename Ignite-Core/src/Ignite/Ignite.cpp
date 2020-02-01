#include "igpch.h"
#include "Ignite/Ignite.h"
#include "Ignite/Log.h"
#include "Ignite/IWindow.h"
#include "Ignite/Renderer/Renderer.h"

namespace Ignite
{
	Ignite::Ignite()
	{
		Log::Init();
		m_application = &Application::Create();
	}

	Ignite::~Ignite()
	{
		int x = 0;
		m_application->Reset();
		Log::Clean();
	}

	
}
