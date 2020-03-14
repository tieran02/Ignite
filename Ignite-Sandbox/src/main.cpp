#include <memory>
#include "Ignite/Ignite.h"
#include <chrono>
#include "Ignite/Events/MouseEvent.h"

class ExampleLayer : public Ignite::Layer
{
public:
	ExampleLayer() : Layer("Example") {}

	void OnAttach() override
	{
		Ignite::PipelineInputLayout layout =
		{
			{ Ignite::PipelineDataType::eFloat3, "a_Position" },
			{ Ignite::PipelineDataType::eFloat3, "a_Normal" },
			{ Ignite::PipelineDataType::eFloat2, "a_TexCoord" },
			{ Ignite::PipelineDataType::eFloat3, "a_Tangent" },
			{ Ignite::PipelineDataType::eFloat3, "a_Bitangent" },
		};

		pipeline = Ignite::IPipeline::Create("shader", "resources/shaders/vert.spv", "resources/shaders/frag.spv", layout);
		unlitPipeline = Ignite::IPipeline::Create("unlit", "resources/shaders/unlitVert.spv", "resources/shaders/unlitFrag.spv", layout);

		//create texture
		std::shared_ptr<Ignite::ITexture2D> image = Ignite::ITexture2D::Create("texture", "resources/textures/texture.jpg", Ignite::TextureType::eDIFFUSE);
		std::shared_ptr<Ignite::ITexture2D> chalet = Ignite::ITexture2D::Create("chalet", "resources/textures/chalet.jpg", Ignite::TextureType::eDIFFUSE);

		material = Ignite::IMaterial::Create("defaultMaterial");

		//load model with default texture
		model = Ignite::Model::Load("resources/models/sponza", "sponza.obj");
		
		//m_sceneUBO.view = glm::lookAt(glm::vec3(10.75f, 10.0f, 0), glm::vec3(0.0f, 8.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		m_sceneUBO.proj = glm::perspective(glm::radians(75.0f), (float)Ignite::Application::Instance().Window()->Width() / (float)Ignite::Application::Instance().Window()->Height(), 0.1f, 50000.0f);
		m_sceneUBO.proj[1][1] *= -1;
		
		//m_sceneUBO.model = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	void OnDetach() override
	{
	}

	void OnUpdate() override
	{
		//rotate
		//static auto startTime = std::chrono::high_resolution_clock::now();

		//auto currentTime = std::chrono::high_resolution_clock::now();
		//float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
		//m_sceneUBO.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//m_sceneUBO.model = glm::rotate(m_sceneUBO.model, glm::radians(90.0f), glm::vec3(0, 1, 0));
		m_sceneUBO.light_position = glm::vec3(0, 1000.0f, 0);
	
		//camera
		if (Ignite::Input::IsKeyPressed(IG_KEY_W))
			camera.Translate(Ignite::CameraDirection::eFORWARD, 0.16);
		if (Ignite::Input::IsKeyPressed(IG_KEY_S))
			camera.Translate(Ignite::CameraDirection::eBACKWARD, 0.16);
		if (Ignite::Input::IsKeyPressed(IG_KEY_A))
			camera.Translate(Ignite::CameraDirection::eLEFT, 0.16);
		if (Ignite::Input::IsKeyPressed(IG_KEY_D))
			camera.Translate(Ignite::CameraDirection::eRIGHT, 0.16);

		camera.MousePosition(Ignite::Input::GetMouseX(), Ignite::Input::GetMouseY());

		m_sceneUBO.view = camera.GetViewMatrix();
		m_sceneUBO.view_pos = camera.Position();

		//start scene
		Ignite::RenderCommand::SetClearColor(glm::vec4{ .5f,.2f,.2f,1.0f });

		Ignite::RenderCommand::SetSceneUniformBuffer(m_sceneUBO);

		Ignite::Renderer::BeginScene(camera);

		Ignite::Renderer::Submit(unlitPipeline.get(), model.get());

		Ignite::Renderer::EndScene();

		Ignite::Renderer::SwapBuffers();

		//FL_LOG_INFO("ExampleLayer::Update");
		if (Ignite::Input::IsKeyPressed(IG_KEY_ESCAPE))
		{
			Ignite::Application::Instance().Close();
		}
	}

	void OnEvent(Ignite::Event& event) override
	{
		//FL_LOG_TRACE("{0}", event);
		if (event.GetEventType() == Ignite::EventType::MouseMoved)
		{
			Ignite::MouseMovedEvent& mouseEvent = (Ignite::MouseMovedEvent&)event;
		}
	}

private:
	std::shared_ptr<Ignite::IPipeline> pipeline;
	std::shared_ptr<Ignite::IPipeline> unlitPipeline;
	std::shared_ptr<Ignite::IMaterial> material;

	std::shared_ptr<Ignite::Model> model;
	std::shared_ptr<Ignite::Model> nanoModel;
	std::shared_ptr<Ignite::IMesh> mesh;
	Ignite::SceneUniformBuffer m_sceneUBO;

	Ignite::Camera camera{ glm::vec3(0,0,0) };
	float lastMouseX = 0;
	float lastMouseY = 0;
};

int main()
{
	Ignite::Ignite ignite;
	ignite.App().PushLayer(new ExampleLayer);

	ignite.App().Start(1920, 1080);

	return 0;
}
