#pragma once
#include "glm/glm.hpp"

namespace Ignite
{
	constexpr uint32_t MAX_LIGHTS{ 10 };
	
	struct Light
	{
		alignas(16) glm::vec4 Position{}; //The W is 0 it is a directional light
		alignas(16) glm::vec3 Intensity{1}; //Colour
		alignas(16) glm::vec3 ConeDirection{};
		alignas(4) float Attenuation{};
		alignas(4) float AmbientCoefficient{};
		alignas(4) float coneAngle{};
	};

	struct LightBuffer
	{
		alignas(4) uint32_t LightCount;
		const Light Lights[MAX_LIGHTS];
	};
}

