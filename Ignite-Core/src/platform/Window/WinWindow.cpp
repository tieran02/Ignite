#include "igpch.h"
#include "platform/Window/WinWindow.h"
#include "Ignite/Log.h"

Ignite::WinWindow::WinWindow(uint32_t width, uint32_t height) : IWindow(width, height)
{
	WinWindow::Init();
}

Ignite::WinWindow::~WinWindow()
{
	WinWindow::Cleanup();
}

void* Ignite::WinWindow::GetHandle() const
{
	return m_glfwWindow;
}

void Ignite::WinWindow::PollEvents()
{
	glfwPollEvents();
}

bool Ignite::WinWindow::ShouldClose() const
{
	return glfwWindowShouldClose(m_glfwWindow);
}

void Ignite::WinWindow::Init()
{
	LOG_CORE_INFO("Creating Windows Window");
	
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	m_glfwWindow = glfwCreateWindow(m_width, m_height, "Ignite Engine", nullptr, nullptr);
}

void Ignite::WinWindow::Cleanup()
{
	LOG_CORE_INFO("Cleaning-up Windows Window");
	glfwDestroyWindow(m_glfwWindow);
	glfwTerminate();
}
