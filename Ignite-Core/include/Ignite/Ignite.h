#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "Ignite/Input.h"
#include "Ignite/Renderer/IPipeline.h"
#include "Ignite/Renderer/RenderCommand.h"
#include "Ignite/Renderer/Renderer.h"
#include "Ignite/Application.h"
#include "Ignite/Renderer/IBuffer.h"
#include "Ignite/Renderer/PipelineInputLayout.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Ignite/Renderer/ITexture2D.h"
#include "Ignite/Renderer/IModel.h"
#include "Ignite/Renderer/ModelLoader.h"

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
