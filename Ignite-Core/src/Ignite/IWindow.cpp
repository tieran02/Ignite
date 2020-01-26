#include "igpch.h"
#include "Ignite/IWindow.h"
#include "platform/Window/WinWindow.h"
#include "Ignite/Log.h"

namespace Ignite
{
	std::unique_ptr<IWindow> IWindow::s_instance = nullptr;
	
	IWindow& IWindow::Create()
	{
		if(s_instance == nullptr)
		{
			LOG_CORE_INFO("Creating IWindow");
			s_instance = std::unique_ptr<WinWindow>(new WinWindow);
		}
		else
		{
			LOG_CORE_WARNING("Window instance alredy exists, Only a single window is allowed");
		}
		return *s_instance;
	}

	IWindow& IWindow::GetInstance()
	{

		CORE_ASSERT(s_instance != nullptr, "Window instance is null");
		return *s_instance;
	}

	void IWindow::Close()
	{
		CORE_ASSERT(s_instance, "Cannot close window, Window is null");
		s_instance = nullptr;
	}
}
