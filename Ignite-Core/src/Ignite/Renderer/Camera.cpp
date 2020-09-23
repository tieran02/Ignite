#include "igpch.h"
#include "Ignite/Renderer/Camera.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Ignite
{
	Camera::Camera(const glm::vec3& origin)
		: m_speed(5.5f),
		m_position(origin),
		m_front(0.0f, 0.0f, -1.0f),
		m_up(0.0f, 1.0f, 0.0f)
	{
	}

	void Camera::Translate(CameraDirection direction, float speed)
	{
		switch (direction)
		{
		case CameraDirection::eLEFT:
			m_position -= glm::normalize(glm::cross(m_front, m_up)) * speed;
			break;
		case CameraDirection::eRIGHT:
			m_position += glm::normalize(glm::cross(m_front, m_up)) * speed;
			break;
		case CameraDirection::eFORWARD:
			m_position += m_front * speed;
			break;
		case CameraDirection::eBACKWARD:
			m_position -= m_front * speed;
			break;
		}
	}

	void Camera::MousePosition(float xpos, float ypos)
	{
		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
		lastX = xpos;
		lastY = ypos;

		float sensitivity = 0.1f; // change this value to your liking
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		yaw += xoffset;
		pitch += yoffset;

		// make sure that when pitch is out of bounds, screen doesn't get flipped
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		m_front = glm::normalize(front);
	}

	glm::mat4 Camera::GetViewMatrix() const
	{
		return glm::lookAt(m_position, m_position + m_front, m_up);
	}

	const glm::vec3& Camera::Position() const
	{
		return m_position;
	}

	const glm::vec3& Camera::Front() const
	{
		return m_front;
	}

	glm::vec3& Camera::Position()
	{
		return m_position;
	}

	float& Camera::Yaw()
	{
		return yaw;
	}

	const float& Camera::Yaw() const
	{
		return yaw;
	}

	float& Camera::Pitch()
	{
		return pitch;
	}

	const float& Camera::Pitch() const
	{
		return pitch;
	}
}
