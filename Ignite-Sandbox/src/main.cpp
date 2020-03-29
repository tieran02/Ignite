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
			{ Ignite::PipelineDataType::eFloat3, "a_Tangent" },
			{ Ignite::PipelineDataType::eFloat3, "a_Bitangent" },
			{ Ignite::PipelineDataType::eFloat2, "a_TexCoord" }
		};

		pipeline = Ignite::IPipeline::Create("shader", layout, "resources/shaders/vert.spv", "resources/shaders/frag.spv");
		unlitPipeline = Ignite::IPipeline::Create("unlit", layout, "resources/shaders/unlitVert.spv", "resources/shaders/unlitFrag.spv");
		debugNormalPipeline = Ignite::IPipeline::Create("debugNormal", layout, 
			"resources/shaders/debugNormalVert.spv",
			"resources/shaders/debugNormalFrag.spv",
			"resources/shaders/debugNormalGeom.spv");

		material = Ignite::IMaterial::Create("defaultMaterial");

		//load model with default texture
		model = Ignite::Model::Load("resources/models/sponza", "sponza.obj");
		//cube model
		cubeModel = Ignite::Model::Load("resources/models", "cube.obj");
		
		lastPrintTime = std::chrono::high_resolution_clock::now();

		//directional light
		lights.emplace_back(Ignite::Light{ glm::normalize(glm::vec4(0.6f,1,0,0)) , glm::vec3(.8,.4,.4) });
		//point light
		lights.emplace_back(Ignite::Light{ glm::vec4(lightPosition,1) , glm::vec3(.25f,.25f,5.0f) ,glm::vec3(0),500,});
		lights.emplace_back(Ignite::Light{ glm::vec4(lightPosition.x,lightPosition.y,lightPosition.z - 500,1) , glm::vec3(5.0,.25f,.25f) ,glm::vec3(0),500, });
		lights.emplace_back(Ignite::Light{ glm::vec4(lightPosition.x,lightPosition.y,lightPosition.z + 500,1) , glm::vec3(.25f,5.0f,.25f) ,glm::vec3(0),500, });
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

		lightPosition.x = 1200 * sin(std::chrono::duration_cast<ms>(currentTime.time_since_epoch()).count() / 1000);
		lights[1].Position.x = lightPosition.x;
		lights[2].Position.x = lightPosition.x;
		lights[3].Position.x = lightPosition.x;
		
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

		Ignite::Renderer::BeginScene(camera,lights);

		Ignite::Renderer::Submit(pipeline.get(), model.get(), glm::translate(glm::mat4(1), glm::vec3(0, 0, 0)));
		//Ignite::Renderer::Submit(debugNormalPipeline.get(), model.get(), glm::translate(glm::mat4(1), glm::vec3(0, 0, 0)));

		//Ignite::Renderer::Submit(debugNormalPipeline.get(), model.get(), glm::translate(glm::mat4(1), glm::vec3(0, 0, 2500)));
		Ignite::Renderer::Submit(unlitPipeline.get(), model.get(), glm::translate(glm::mat4(1), glm::vec3(0,0,2500)));

		//render light
		Ignite::Renderer::Submit(unlitPipeline.get(), cubeModel.get(), glm::translate(glm::mat4(1), lightPosition));
		
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
	std::shared_ptr<Ignite::IPipeline> debugNormalPipeline;
	std::shared_ptr<Ignite::IMaterial> material;

	std::shared_ptr<Ignite::Model> model;
	std::shared_ptr<Ignite::Model> cubeModel;
	std::shared_ptr<Ignite::IMesh> mesh;
	std::vector<Ignite::Light> lights;

	glm::vec3 lightPosition{ 50,100,-8 };
	Ignite::Camera camera{ glm::vec3(0,0,0) };
};

int main()
{
	Ignite::Ignite ignite;
	ignite.App().PushLayer(new ExampleLayer);

	ignite.App().Start(1920, 1080);

	return 0;
}
