#include "igpch.h"
#include "Ignite/Renderer/Light.h"

#include "Ignite/Log.h"

Ignite::LightBuffer::LightBuffer(const std::vector<Light>& lights)
{
	CORE_ASSERT(lights.size() <= MAX_LIGHTS, "Exceded max light count!");

	//Cap light count incase lights exceed the max light count (only copies upto the max light count)
	LightCount = std::min(MAX_LIGHTS, lights.size());
	//copy lights to light array
	for (size_t i = 0; i < LightCount; i++)
	{
		Lights[i] = lights[i];
	}
}

size_t Ignite::LightBuffer::Size() const
{
	return sizeof(LightCount) + (sizeof(Light) * LightCount);
}
