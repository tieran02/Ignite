#include <memory>
#include "Ignite/Ignite.h"
#include <chrono>
#include "Ignite/Events/MouseEvent.h"

class ExampleLayer : public Ignite::Layer
{
public:
	ExampleLayer() : Layer("Example") {}

	float lastPrintTime;
	float lastFrameTime;
	int nbFrames = 0;
	float deltaTime = 16.0f;
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

		material = Ignite::IMaterial::Create("defaultMaterial");

		//load model with default texture
		model = Ignite::Model::Load("resources/models/sponza", "sponza.obj");
		
		lastPrintTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	}

	void OnDetach() override
	{
	}

	void OnUpdate() override
	{
		//rotate
		float currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		deltaTime = currentTime - lastFrameTime;
		lastFrameTime = currentTime;
		nbFrames++;

		if (currentTime - lastPrintTime >= 1000.0f)
		{ 
			printf("%i fps	", nbFrames);
			printf("%f ms\n", deltaTime);
			nbFrames = 0;
			lastPrintTime += 1000.0;
		}
		
		Ignite::Renderer::SceneUBO().light_position = glm::vec3(0, 1000.0f, 0);
	
		//camera
		if (Ignite::Input::IsKeyPressed(IG_KEY_W))
			camera.Translate(Ignite::CameraDirection::eFORWARD, deltaTime);
		if (Ignite::Input::IsKeyPressed(IG_KEY_S))
			camera.Translate(Ignite::CameraDirection::eBACKWARD, deltaTime);
		if (Ignite::Input::IsKeyPressed(IG_KEY_A))
			camera.Translate(Ignite::CameraDirection::eLEFT, deltaTime);
		if (Ignite::Input::IsKeyPressed(IG_KEY_D))
			camera.Translate(Ignite::CameraDirection::eRIGHT, deltaTime);

		camera.MousePosition(Ignite::Input::GetMouseX(), Ignite::Input::GetMouseY());

		//start scene
		Ignite::RenderCommand::SetClearColor(glm::vec4{ .5f,.2f,.2f,1.0f });

		Ignite::Renderer::BeginScene(camera);

		Ignite::Renderer::Submit(pipeline.get(), model.get(), glm::translate(glm::mat4(1), glm::vec3(0, 0, 0)));
		Ignite::Renderer::Submit(unlitPipeline.get(), model.get(), glm::translate(glm::mat4(1), glm::vec3(0,0,2500)));

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
	std::shared_ptr<Ignite::IMesh> mesh;

	Ignite::Camera camera{ glm::vec3(0,0,0) };
};

int main()
{
	Ignite::Ignite ignite;
	ignite.App().PushLayer(new ExampleLayer);

	ignite.App().Start(1920, 1080);

	return 0;
}
