#include "igpch.h"
#include "platform/Window/WinInput.h"
#include "platform/Window/WinWindow.h"
#include "Ignite/Application.h"

namespace Ignite
{
	Input* Input::s_instance = new WinInput;

	bool WinInput::IsKeyPressedImpl(int keyCode)
	{
		const auto window = static_cast<GLFWwindow*>(Application::Instance().Window()->GetHandle());
		const auto state = glfwGetKey(window, keyCode);

		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool WinInput::IsMousePressedImpl(int button)
	{
		const auto window = static_cast<GLFWwindow*>(Application::Instance().Window()->GetHandle());
		const auto state = glfwGetMouseButton(window, button);

		return state == GLFW_PRESS;
	}

	float WinInput::GetMouseXImpl()
	{
		auto [x, y] = GetMousePositionImpl();
		return x;
	}

	float WinInput::GetMouseYImpl()
	{
		auto [x, y] = GetMousePositionImpl();
		return y;
	}

	std::pair<float, float> WinInput::GetMousePositionImpl()
	{
		const auto window = static_cast<GLFWwindow*>(Application::Instance().Window()->GetHandle());

		double x, y;
		glfwGetCursorPos(window, &x, &y);

		return std::make_pair((float)x, (float)y);
	}
}
