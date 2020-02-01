#pragma once
#include "Ignite/IWindow.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Ignite
{
	class WinWindow : public IWindow
	{
		friend class IWindow;
	protected:
		WinWindow(const WindowProperites& properites);
		void Init() override;
		void Cleanup() override;
	public:
		~WinWindow();

		void* GetHandle() const override;
		void OnUpdate() override;
		void Close() override;
		void SetEventCallback(const EventCallbackFn& callback) override;
	private:
		GLFWwindow* m_glfwWindow;
	};
}
