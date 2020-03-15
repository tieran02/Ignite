#include <memory>
#include "Ignite/Ignite.h"
#include <chrono>
#include "Ignite/Events/MouseEvent.h"

using ms = std::chrono::duration<float, std::milli>;

class ExampleLayer : public Ignite::Layer
{
public:
	ExampleLayer() : Layer("Example") {}

	std::chrono::high_resolution_clock::time_point lastPrintTime;
	std::chrono::high_resolution_clock::time_point lastFrameTime;
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
		
		lastPrintTime = std::chrono::high_resolution_clock::now();
	}

	void OnDetach() override
	{
	}

	void OnUpdate() override
	{
		//rotate
		std::chrono::high_resolution_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
		
		deltaTime = std::chrono::duration_cast<ms>(currentTime - lastFrameTime).count();
		lastFrameTime = currentTime;
		nbFrames++;

		if (std::chrono::duration_cast<ms>(currentTime - lastPrintTime).count() >= 1000.0f)
		{ 
			printf("%i fps	", nbFrames);
			printf("%f ms\n", deltaTime);
			nbFrames = 0;
			lastPrintTime = currentTime;
		}
		
		Ignite::Renderer::SceneUBO().light_position = glm::vec3(0, 1000.0f, 0);

		constexpr float CAMERA_SPEED = 0.5f;
		//camera
		if (Ignite::Input::IsKeyPressed(IG_KEY_W))
			camera.Translate(Ignite::CameraDirection::eFORWARD, CAMERA_SPEED * deltaTime);
		if (Ignite::Input::IsKeyPressed(IG_KEY_S))
			camera.Translate(Ignite::CameraDirection::eBACKWARD, CAMERA_SPEED * deltaTime);
		if (Ignite::Input::IsKeyPressed(IG_KEY_A))
			camera.Translate(Ignite::CameraDirection::eLEFT, CAMERA_SPEED * deltaTime);
		if (Ignite::Input::IsKeyPressed(IG_KEY_D))
			camera.Translate(Ignite::CameraDirection::eRIGHT, CAMERA_SPEED * deltaTime);

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
