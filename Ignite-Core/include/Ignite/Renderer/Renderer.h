#pragma once
#include "GraphicsContext.h"
#include "glm/mat4x4.hpp"
#include "Mesh.h"
#include "Camera.h"
#include "Light.h"

namespace Ignite {
	class Model;
	class Pipeline;

	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();

		static bool IsInitialised();

		//static void OnWindowResize(uint32_t width, uint32_t height);

		static void BeginScene(const Camera& camera, const std::vector<LightData>& lights);
		static void EndScene();

		//static void Submit(const Pipeline& pipeline, const Ref<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));
		static void Submit(const Ref<Pipeline>& pipeline, const Ref<Mesh>& mesh, const glm::mat4& transform = glm::mat4(1.0f));
		static void Submit(const Ref<Pipeline>& pipeline, const Ref<Model>& model, const glm::mat4& transform = glm::mat4(1.0f));

		static void SwapBuffers();

		static GraphicsContext* GraphicsContext();
		static SceneUniformBuffer& SceneUBO();
	private:
		static bool m_recordingScene;
		static SceneUniformBuffer m_sceneUBO;

		static void submitMesh(const Ref<Pipeline>& pipeline, const Ref<Mesh>& mesh, const glm::mat4& transform);
	};
}
