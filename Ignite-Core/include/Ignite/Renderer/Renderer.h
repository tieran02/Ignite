#pragma once
#include "IGraphicsContext.h"
#include "glm/mat4x4.hpp"
#include "IMesh.h"
#include "Camera.h"

namespace Ignite {
	class Model;
	class IPipeline;

	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();

		static bool IsInitialised();

		//static void OnWindowResize(uint32_t width, uint32_t height);

		static void BeginScene(const Camera& camera);
		static void EndScene();

		//static void Submit(const IPipeline& pipeline, const Ref<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));
		static void Submit(const IPipeline* pipeline, const IMesh* mesh, const glm::mat4& transform = glm::mat4(1.0f));
		static void Submit(const IPipeline* pipeline, const Model* model, const glm::mat4& transform = glm::mat4(1.0f));

		static void SwapBuffers();

		static IGraphicsContext* GraphicsContext();
	private:
		static bool m_recordingScene;

		static void submitMesh(const ::Ignite::IPipeline* pipeline, const ::Ignite::IMesh* mesh, const glm::mat4&
			                       transform = glm::mat4(1.0f));
	};
}
