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
		WinWindow(uint32_t width, uint32_t height);
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
