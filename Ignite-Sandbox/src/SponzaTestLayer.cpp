#include "SponzaTestLayer.h"
#include <chrono>

void SponzaTestLayer::OnAttach()
{
	Ignite::PipelineInputLayout layout =
	{
		{ Ignite::PipelineDataType::eFloat3, "a_Position" },
		{ Ignite::PipelineDataType::eFloat3, "a_Normal" },
		{ Ignite::PipelineDataType::eFloat4, "a_Tangent" },
		{ Ignite::PipelineDataType::eFloat2, "a_TexCoord" }
	};

	Ignite::PipelineInfo litPipelineInfo{ "shader", layout, "resources/shaders/vert.spv", "resources/shaders/frag.spv" };
	Ignite::PipelineInfo unlitPipelineInfo{ "unlitShader", layout, "resources/shaders/unlitVert.spv", "resources/shaders/unlitFrag.spv" };
	pipeline = Ignite::IPipeline::Create(litPipelineInfo);
	unlitPipeline = Ignite::IPipeline::Create(unlitPipelineInfo);

	//load model with default texture
	sponzaModel = Ignite::Model::Create(Ignite::ModelInfo{ "sponza","resources/models/sponza", "sponza.obj" });
	//cube model
	cubeModel = Ignite::Model::Create(Ignite::ModelInfo{ "cube","resources/models", "cube.obj" });

	lastPrintTime = std::chrono::high_resolution_clock::now();

	//directional light
	lights.emplace_back(Ignite::Light{ glm::normalize(glm::vec4(0.6f,1,0,0)) , glm::vec3(.8,.4,.4) });
	//point light
	lights.emplace_back(Ignite::Light{ glm::vec4(lightPosition,1) , glm::vec3(.25f,.25f,5.0f) ,glm::vec3(0),500, });
	lights.emplace_back(Ignite::Light{ glm::vec4(lightPosition.x,lightPosition.y,lightPosition.z - 500,1) , glm::vec3(5.0,.25f,.25f) ,glm::vec3(0),500, });
	lights.emplace_back(Ignite::Light{ glm::vec4(lightPosition.x,lightPosition.y,lightPosition.z + 500,1) , glm::vec3(.25f,5.0f,.25f) ,glm::vec3(0),500, });
	lights.emplace_back(Ignite::Light{ glm::vec4(lightPosition.x,lightPosition.y,lightPosition.z + 500,1) , glm::vec3(.25f,5.0f,.25f) ,glm::vec3(0),500, });
	lights.emplace_back(Ignite::Light{ glm::vec4(lightPosition.x,lightPosition.y,lightPosition.z + 500,1) , glm::vec3(.25f,5.0f,.25f) ,glm::vec3(0),500, });
	lights.emplace_back(Ignite::Light{ glm::vec4(lightPosition.x,lightPosition.y,lightPosition.z + 500,1) , glm::vec3(.25f,5.0f,.25f) ,glm::vec3(0),500, });
	lights.emplace_back(Ignite::Light{ glm::vec4(lightPosition.x,lightPosition.y,lightPosition.z + 500,1) , glm::vec3(.25f,5.0f,.25f) ,glm::vec3(0),500, });
	lights.emplace_back(Ignite::Light{ glm::vec4(lightPosition.x,lightPosition.y,lightPosition.z + 500,1) , glm::vec3(.25f,5.0f,.25f) ,glm::vec3(0),500, });
	lights.emplace_back(Ignite::Light{ glm::vec4(lightPosition.x,lightPosition.y,lightPosition.z + 500,1) , glm::vec3(.25f,5.0f,.25f) ,glm::vec3(0),500, });
	lights.emplace_back(Ignite::Light{ glm::vec4(lightPosition.x,lightPosition.y,lightPosition.z + 500,1) , glm::vec3(.25f,5.0f,.25f) ,glm::vec3(0),500, });
	lights.emplace_back(Ignite::Light{ glm::vec4(lightPosition.x,lightPosition.y,lightPosition.z + 500,1) , glm::vec3(.25f,5.0f,.25f) ,glm::vec3(0),500, });
	lights.emplace_back(Ignite::Light{ glm::vec4(lightPosition.x,lightPosition.y,lightPosition.z + 500,1) , glm::vec3(.25f,5.0f,.25f) ,glm::vec3(0),500, });
	lights.emplace_back(Ignite::Light{ glm::vec4(lightPosition.x,lightPosition.y,lightPosition.z + 500,1) , glm::vec3(.25f,5.0f,.25f) ,glm::vec3(0),500, });
	lights.emplace_back(Ignite::Light{ glm::vec4(lightPosition.x,lightPosition.y,lightPosition.z + 500,1) , glm::vec3(.25f,5.0f,.25f) ,glm::vec3(0),500, });
	lights.emplace_back(Ignite::Light{ glm::vec4(lightPosition.x,lightPosition.y,lightPosition.z + 500,1) , glm::vec3(.25f,5.0f,.25f) ,glm::vec3(0),500, });
}

void SponzaTestLayer::OnDetach()
{
}

void SponzaTestLayer::OnUpdate()
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
	//lights[2].Position.x = lightPosition.x;
	//lights[3].Position.x = lightPosition.x;

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

	Ignite::Renderer::BeginScene(camera, lights);

	Ignite::Renderer::Submit(pipeline.get(), sponzaModel.get(), glm::translate(glm::mat4(1), glm::vec3(0, 0, 0)));

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

void SponzaTestLayer::OnEvent(Ignite::Event& event)
{
}
