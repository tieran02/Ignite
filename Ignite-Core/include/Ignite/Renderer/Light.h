#pragma once
#include "glm/glm.hpp"

namespace Ignite
{
	struct Light
	{
		glm::vec4 Position{}; //The W is 0 it is a directional light
		glm::vec3 Intensity{1}; //Colour
		glm::vec3 ConeDirection{};
		float Attenuation{};
		float AmbientCoefficient{};
		float coneAngle{};
	};
}

