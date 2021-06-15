#include "igpch.h"
#include "Ignite/Renderer/Light.h"

#include "Ignite/Log.h"

Ignite::LightBuffer::LightBuffer(const std::vector<LightData>& lights)
{
	CORE_ASSERT(lights.size() <= MAX_LIGHTS, "Exceded max light count!");

	//Cap light count incase lights exceed the max light count (only copies upto the max light count)
	LightCount = std::min(MAX_LIGHTS, static_cast<uint32_t>(lights.size()));
	//copy lights to light array
	for (size_t i = 0; i < LightCount; i++)
	{
		Lights[i] = lights[i];
	}
}

size_t Ignite::LightBuffer::Size() const
{
	return sizeof(LightCount) + (sizeof(LightData) * LightCount);
}

Ignite::Light::Light(LightType type, const glm::vec3& origin, const glm::vec3& color) : SceneObject(SceneObjectType::LIGHT)
{
	switch (type)
	{
	case LightType::DIRECTIONAL:
		m_lightData.Position = glm::vec4(origin, 0.0f);
		break;
	case LightType::POINT:
		m_lightData.Position = glm::vec4(origin, 1.0f);
		break;
	case LightType::SPOT:
		m_lightData.Position = glm::vec4(origin, 1.0f);
		break;
	default: 
		m_lightData.Position = glm::vec4(origin, 1.0f);;
	}

	m_lightData.Intensity = color;
}
