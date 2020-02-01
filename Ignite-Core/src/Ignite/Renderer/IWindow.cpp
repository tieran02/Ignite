#include "igpch.h"
#include "Ignite/IWindow.h"
#include "platform/Window/WinWindow.h"
#include "Ignite/Log.h"

namespace Ignite
{
	IWindow::IWindow(const WindowProperites& properites) : m_properites(properites)
	{
	}

	std::unique_ptr<IWindow> IWindow::Create(const WindowProperites& properites)
	{
		LOG_CORE_INFO("Creating IWindow");
		return std::unique_ptr<WinWindow>(new WinWindow(properites));
	}
}
