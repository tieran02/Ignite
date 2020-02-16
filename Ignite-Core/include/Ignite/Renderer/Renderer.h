#pragma once
#include "IGraphicsContext.h"
#include "glm/mat4x4.hpp"
#include "IMesh.h"

namespace Ignite {
	class IPipeline;

	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();

		static bool IsInitialised();

		//static void OnWindowResize(uint32_t width, uint32_t height);

		static void BeginScene();
		static void EndScene();

		//static void Submit(const IPipeline& pipeline, const Ref<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));
		static void Submit(const ::Ignite::IPipeline* pipeline, const ::Ignite::IMesh* model, const glm::mat4& transform = glm::mat4(1.0f));

		static void SwapBuffers();

		static IGraphicsContext* GraphicsContext();
	private:
		static bool m_recordingScene;
	};
}
