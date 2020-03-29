#pragma once
#include "glm/glm.hpp"

namespace Ignite
{
	constexpr uint32_t MAX_LIGHTS{ 10 };
	
	struct Light
	{
		alignas(16) glm::vec4 Position{1}; //The W is 0 it is a directional light
		alignas(16) glm::vec3 Intensity{1}; //Colour
		alignas(16) glm::vec3 ConeDirection{0};
		alignas(4)  float Attenuation{0.1};
		alignas(4)  float AmbientCoefficient{0};
		alignas(4)  float coneAngle{10};
	};

	struct LightBuffer
	{
		alignas(4) uint32_t LightCount;
		alignas(16) Light Lights[MAX_LIGHTS];
		
		///Light buffer has a limit and only the first lights before the limit would be sent to the shader
		///Something to consider is to get the closest lights and directional lights first
		LightBuffer(const std::vector<Light>& lights)
		{
			constexpr size_t MAX_LIGHTS = 10;
			
			LightCount = std::min(MAX_LIGHTS, lights.size());
			//copy lights to light array
			for (size_t i = 0; i < LightCount; i++)
			{
				Lights[i] = lights[i];
			}
		}

		
		size_t Size() const
		{
			return sizeof(LightCount) + (sizeof(Light) * LightCount);
		}
	};
}

