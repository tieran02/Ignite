#include "TangentTestLayer.h"

void TangentTestLayer::OnAttach()
{
	Ignite::PipelineInputLayout layout =
	{
		{Ignite::PipelineDataType::eFloat3, "a_Position"},
		{Ignite::PipelineDataType::eFloat3, "a_Normal"},
		{Ignite::PipelineDataType::eFloat4, "a_Tangent"},
		{Ignite::PipelineDataType::eFloat2, "a_TexCoord"}
	};

	Ignite::PipelineInfo litPipelineInfo{ "shader", layout,
		"resources/shaders/vert.spv",
		"resources/shaders/frag.spv" };
	Ignite::PipelineInfo normalPipelineInfo{ "normal", layout,
		"resources/shaders/normalVert.spv",
		"resources/shaders/normalFrag.spv" };
	Ignite::PipelineInfo normalMappingPipelineInfo{ "normalMapping", layout,
		"resources/shaders/normalMappingVert.spv",
		"resources/shaders/normalMappingFrag.spv" };
	Ignite::PipelineInfo debugNormalPipelineInfo{ "debugNormal", layout,
			"resources/shaders/debugNormalVert.spv",
			"resources/shaders/debugNormalFrag.spv",
			"resources/shaders/debugNormalGeom.spv" };
	
	pipeline = Ignite::IPipeline::Create(litPipelineInfo);
	normalPipeline = Ignite::IPipeline::Create(normalPipelineInfo);
	normalMappingPipeline = Ignite::IPipeline::Create(normalMappingPipelineInfo);
	debugNormalPipeline = Ignite::IPipeline::Create(debugNormalPipelineInfo);

	//load model with default texture
	tangentModel = Ignite::Model::Load("resources/models/tangent", "NormalTangentTestPlatform.obj");
	//cube model
	cubeModel = Ignite::Model::Load("resources/models", "cube.obj");

	lastPrintTime = std::chrono::high_resolution_clock::now();

	//directional light
	lights.emplace_back(Ignite::Light{glm::normalize(glm::vec4(0.6f, 1, 0, 0)), glm::vec3(.8, .4, .4)});

	camera.Yaw() = -90.0f;
	camera.Pitch() = -90.0f;
	camera.Position() = glm::vec3(0, 150, 50);
}

void TangentTestLayer::OnDetach()
{
}

void TangentTestLayer::OnUpdate()
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

	constexpr float CAMERA_SPEED = 0.05f;
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
	Ignite::RenderCommand::SetClearColor(glm::vec4{.5f, .2f, .2f, 1.0f});

	Ignite::Renderer::BeginScene(camera, lights);

	glm::mat4 model(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(20.0f, 20, 20.0f));
	Ignite::Renderer::Submit(pipeline.get(), tangentModel.get(), model);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 50.0f));
	model = glm::scale(model, glm::vec3(20.0f, 20, 20.0f));
	Ignite::Renderer::Submit(normalPipeline.get(), tangentModel.get(), model);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(50.0f, 0.0f, 50.0f));
	model = glm::scale(model, glm::vec3(20.0f, 20, 20.0f));
	Ignite::Renderer::Submit(normalMappingPipeline.get(), tangentModel.get(), model);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-50.0f, 0.0f, 50.0f));
	model = glm::scale(model, glm::vec3(20.0f, 20, 20.0f));
	Ignite::Renderer::Submit(normalMappingPipeline.get(), tangentModel.get(), model);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 100.0f));
	model = glm::scale(model, glm::vec3(20.0f, 20, 20.0f));
	Ignite::Renderer::Submit(debugNormalPipeline.get(), tangentModel.get(), model);
	Ignite::Renderer::Submit(pipeline.get(), tangentModel.get(), model);

	//render light
	//Ignite::Renderer::Submit(unlitPipeline.get(), cubeModel.get(), glm::translate(glm::mat4(1), lightPosition));

	Ignite::Renderer::EndScene();

	Ignite::Renderer::SwapBuffers();

	//FL_LOG_INFO("ExampleLayer::Update");
	if (Ignite::Input::IsKeyPressed(IG_KEY_ESCAPE))
	{
		Ignite::Application::Instance().Close();
	}
}

void TangentTestLayer::OnEvent(Ignite::Event& event)
{
}
