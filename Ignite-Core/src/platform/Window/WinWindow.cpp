#include "igpch.h"
#include "platform/Window/WinWindow.h"
#include "Ignite/Log.h"
#include "Ignite/Events/ApplicationEvent.h"
#include <Ignite\Events\KeyEvent.h>
#include <Ignite\Events\MouseEvent.h>

static void GLFWErrorCallback(int error, const char* description)
{
	LOG_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
}

Ignite::WinWindow::WinWindow(const WindowProperites& properites) : IWindow(properites)
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

void Ignite::WinWindow::OnUpdate()
{
	glfwPollEvents();
}

void Ignite::WinWindow::Close()
{
	Cleanup();
}

void Ignite::WinWindow::SetEventCallback(const EventCallbackFn& callback)
{
	m_properites.EventCallback = callback;
}

void Ignite::WinWindow::Init()
{
	LOG_CORE_INFO("Creating Windows Window");
	
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	glfwSetErrorCallback(GLFWErrorCallback);
	
	m_glfwWindow = glfwCreateWindow(m_properites.Width, m_properites.Height, "Ignite Engine", nullptr, nullptr);
	glfwSetWindowUserPointer(m_glfwWindow, &m_properites);

	//callback events
	
	// Set GLFW callbacks
	glfwSetWindowSizeCallback(m_glfwWindow, [](GLFWwindow* window, int width, int height)
		{
			WindowProperites& data = *(WindowProperites*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;

			WindowResizeEvent event(width, height);
			data.EventCallback(event);
		});

	glfwSetWindowCloseCallback(m_glfwWindow, [](GLFWwindow* window)
		{
			WindowProperites& data = *(WindowProperites*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.EventCallback(event);
		});

	glfwSetKeyCallback(m_glfwWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowProperites& data = *(WindowProperites*)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case GLFW_PRESS:
			{
				KeyPressedEvent event(key, 0);
				data.EventCallback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				KeyReleaseEvent event(key);
				data.EventCallback(event);
				break;
			}
			case GLFW_REPEAT:
			{
				KeyPressedEvent event(key, 1);
				data.EventCallback(event);
				break;
			}
			}
		});

	glfwSetCharCallback(m_glfwWindow, [](GLFWwindow* window, unsigned int keycode)
		{
			WindowProperites& data = *(WindowProperites*)glfwGetWindowUserPointer(window);
			KeyTypedEvent event(keycode);
			data.EventCallback(event);
		});

	glfwSetMouseButtonCallback(m_glfwWindow, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowProperites& data = *(WindowProperites*)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case GLFW_PRESS:
			{
				MouseButtonPressedEvent event(button);
				data.EventCallback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				MouseButtonReleasedEvent event(button);
				data.EventCallback(event);
				break;
			}
			}
		});

	glfwSetScrollCallback(m_glfwWindow, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			WindowProperites& data = *(WindowProperites*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.EventCallback(event);
		});

	glfwSetCursorPosCallback(m_glfwWindow, [](GLFWwindow* window, double xPos, double yPos)
		{
			WindowProperites& data = *(WindowProperites*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float)xPos, (float)yPos);
			data.EventCallback(event);
		});
}

void Ignite::WinWindow::Cleanup()
{
	LOG_CORE_INFO("Cleaning-up Windows Window");
	glfwDestroyWindow(m_glfwWindow);
	glfwTerminate();
}
