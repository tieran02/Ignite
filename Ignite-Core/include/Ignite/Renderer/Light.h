#pragma once
#include "glm/glm.hpp"
#include "Ignite/SceneObject.h"

namespace Ignite
{
	constexpr uint32_t MAX_LIGHTS{ 10 };
	
	struct LightData
	{
		alignas(16) glm::vec4 Position{1}; //The W is 0 it is a directional light
		alignas(16) glm::vec3 Intensity{1}; //Colour
		alignas(16) glm::vec3 ConeDirection{0};
		alignas(4)  float Attenuation{0.1f};
		alignas(4)  float AmbientCoefficient{0.0f};
		alignas(4)  float coneAngle{10.0f};
	};

	struct LightBuffer
	{
		alignas(4) uint32_t LightCount;
		alignas(16) LightData Lights[MAX_LIGHTS];
		
		///Light buffer has a limit and only the first lights before the limit would be sent to the shader
		///Something to consider is to get the closest lights and directional lights first
		LightBuffer(const std::vector<LightData>& lights);


		size_t Size() const;
	};

	enum class LightType
	{
		DIRECTIONAL,
		POINT,
		SPOT
	};
	
	class Light : public SceneObject
	{
	public:
		Light(LightType type, const glm::vec3& origin, const glm::vec3& color);
	private:
		LightType m_lightType;
		LightData m_lightData;
		
	};
}

