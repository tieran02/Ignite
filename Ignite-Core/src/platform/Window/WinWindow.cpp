#include "igpch.h"
#include "platform/Window/WinWindow.h"
#include "Ignite/Log.h"

Ignite::WinWindow::WinWindow() : IWindow()
{
	WinWindow::Init();
}

Ignite::WinWindow::~WinWindow()
{
	WinWindow::Cleanup();
}

void Ignite::WinWindow::Init()
{
	LOG_CORE_INFO("Creating Windows Window");
	
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	m_glfwWindow = glfwCreateWindow(1280, 720, "Ignite Engine", nullptr, nullptr);
	
	m_renderer = IRenderer::Create();
}

void Ignite::WinWindow::Cleanup()
{
	m_renderer.reset();
	LOG_CORE_INFO("Cleaning-up Windows Window");
	glfwDestroyWindow(m_glfwWindow);
	glfwTerminate();
}
