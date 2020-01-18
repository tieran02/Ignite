#include "igpch.h"
#include "Ignite/IWindow.h"
#include "platform/Window/WinWindow.h"

namespace Ignite
{
	std::unique_ptr<IWindow> IWindow::Create()
	{
		return std::unique_ptr<WinWindow>(new WinWindow);
	}
}
