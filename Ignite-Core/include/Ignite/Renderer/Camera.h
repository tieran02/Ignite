#pragma once
#include "glm/glm.hpp"

namespace Ignite
{
	enum class CameraDirection
	{
		eFORWARD,
		eBACKWARD,
		eLEFT,
		eRIGHT
	};
	
	class Camera
	{
	public:
		Camera(const glm::vec3& origin);

		void Translate(CameraDirection direction, float deltaTime);

		void MousePosition(double xpos, double ypos);

		glm::mat4 GetViewMatrix() const;
		const glm::vec3& Position() const;
		const glm::vec3& Front() const;
	private:
		float m_speed;
		glm::vec3 m_position;
		glm::vec3 m_front;
		glm::vec3 m_up;

		float yaw = -90.0f;
		float pitch = 0.0f;
		//todo set as width
		float lastX = 1920.0f / 2.0;
		float lastY = 1080.0f / 2.0;
		
		float fov = 65.0f;
	};
}

