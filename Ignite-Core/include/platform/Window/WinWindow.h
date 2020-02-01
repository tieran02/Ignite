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
		WinWindow();
		void Init() override;
		void Cleanup() override;
	public:
		~WinWindow();

		void* GetHandle() const override;
		void PollEvents() override;
		bool ShouldClose() const override;
	private:
		GLFWwindow* m_glfwWindow;
	};
}
