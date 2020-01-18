#include "igpch.h"
#include "platform/Window/WinWindow.h"
#include "Ignite/Log.h"

Ignite::WinWindow::WinWindow() : IWindow()
{
	Init();
}

Ignite::WinWindow::~WinWindow()
{
	Cleanup();
}

void Ignite::WinWindow::Init()
{
	LOG_CORE_INFO("Creating Windows Window");

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
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
