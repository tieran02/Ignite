#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Ignite/Application.h"
#include "Ignite/Input.h"
#include "Ignite/Renderer/Camera.h"
#include "Ignite/Renderer/IBuffer.h"
#include "Ignite/Renderer/IMesh.h"
#include "Ignite/Renderer/IPipeline.h"
#include "Ignite/Renderer/ITexture2D.h"
#include "Ignite/Renderer/Light.h"
#include "Ignite/Renderer/Model.h"
#include "Ignite/Renderer/PipelineInputLayout.h"
#include "Ignite/Renderer/RenderCommand.h"
#include "Ignite/Renderer/Renderer.h"

namespace Ignite
{
	class Ignite
	{
	public:
		Ignite();
		~Ignite();

		Ignite::Application& App() { return *m_application; }
	private:
		Ignite::Application* m_application;
	};
}
