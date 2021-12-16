#include "SponzaTestLayer.h"
#include <chrono>

void SponzaTestLayer::OnAttach()
{
	//Create descriptor sets
	sceneLayout = Ignite::CreateRef<Ignite::DescriptorSetLayout>(Ignite::SetType::UNIFORM_BUFFER, Ignite::StageBitSet{}.set(to_underlying(Ignite::SetBindingStage::VERTEX)));
	sceneLayout->AddVariable(Ignite::PipelineDataType::eMat4);
	sceneLayout->AddVariable(Ignite::PipelineDataType::eMat4);
	sceneLayout->AddVariable(Ignite::PipelineDataType::eFloat3);

	textureLayout = Ignite::CreateRef<Ignite::DescriptorSetLayout>(Ignite::SetType::SAMPLER, Ignite::StageBitSet{}.set(to_underlying(Ignite::SetBindingStage::FRAGMENT)));
	textureLayout->AddVariable(Ignite::PipelineDataType::eInt); //diffuse
	textureLayout->AddVariable(Ignite::PipelineDataType::eInt); //Spec
	textureLayout->AddVariable(Ignite::PipelineDataType::eInt); //Normal
	textureLayout->AddVariable(Ignite::PipelineDataType::eInt); //Alpha

	lightLayout = Ignite::CreateRef<Ignite::DescriptorSetLayout>(Ignite::SetType::STORAGE, Ignite::StageBitSet{}.set(to_underlying(Ignite::SetBindingStage::FRAGMENT)));
	lightLayout->AddVariable(Ignite::PipelineDataType::eInt); //light count
	for (size_t i = 0; i < Ignite::MAX_LIGHTS; i++)
	{
		lightLayout->AddVariable(Ignite::PipelineDataType::eFloat4); // pos
		lightLayout->AddVariable(Ignite::PipelineDataType::eFloat3); // intensities
		lightLayout->AddVariable(Ignite::PipelineDataType::eFloat3); // cone dir
		lightLayout->AddVariable(Ignite::PipelineDataType::eFloat); // attenuation
		lightLayout->AddVariable(Ignite::PipelineDataType::eFloat); // ambientCoefficient
		lightLayout->AddVariable(Ignite::PipelineDataType::eFloat); // coneAngle
	}

	Ignite::Renderer::GraphicsContext()->CreateDescriptorSetLayouts();

	defaultShaderEffect = Ignite::CreateRef<Ignite::ShaderEffect>(std::vector<Ignite::Ref<Ignite::DescriptorSetLayout>>{ sceneLayout, textureLayout, lightLayout});
	defaultShaderEffect->LoadShaderStage(Ignite::SetBindingStage::VERTEX, "resources/shaders/vert.spv");
	defaultShaderEffect->LoadShaderStage(Ignite::SetBindingStage::FRAGMENT, "resources/shaders/frag.spv");

	defaultPass = Ignite::CreateRef<Ignite::ShaderPass>(*Ignite::Renderer::GraphicsContext(), defaultShaderEffect);

	effectTemplate = Ignite::CreateRef<Ignite::EffectTemplate>();
	effectTemplate->SetShaderPass(Ignite::ShaderPassType::FORWARD, defaultPass);

	/*Ignite::BaseMaterialCreateInfo baseMaterialCreateInfo{};
	material = Ignite::BaseMaterial::Create(baseMaterialCreateInfo);
	material->SetParameter<Ignite::MaterialParameterType::DIFFUSE_COLOUR>(glm::vec3(1, 2, 3));

	const glm::vec3* value = material->GetParameter<Ignite::MaterialParameterType::DIFFUSE_COLOUR>();
	material->ClearParameter<Ignite::MaterialParameterType::DIFFUSE_COLOUR>();
	value = material->GetParameter<Ignite::MaterialParameterType::DIFFUSE_COLOUR>();*/

	////load model with default texture
	//sponzaModel = Ignite::Model::Create(Ignite::ModelCreateInfo{ "sponza","resources/models/sponza", "sponza.obj" });
	////cube model
	//cubeModel = Ignite::Model::Create(Ignite::ModelCreateInfo{ "cube","resources/models", "cube.obj" });

	lastPrintTime = std::chrono::high_resolution_clock::now();

	////directional light
	//lights.emplace_back(Ignite::LightData{ glm::normalize(glm::vec4(0.4f,1,0.4f,0)) , glm::vec3(1.2f,.8f,.8f) });
	////point light
	//lights.emplace_back(Ignite::LightData{ glm::vec4(lightPosition,1) , glm::vec3(.25f,.25f,5.0f) ,glm::vec3(0),500, });
	//lights.emplace_back(Ignite::LightData{ glm::vec4(lightPosition.x,lightPosition.y,lightPosition.z - 500,1) , glm::vec3(5.0,.25f,.25f) ,glm::vec3(0),500, });
	//lights.emplace_back(Ignite::LightData{ glm::vec4(lightPosition.x,lightPosition.y,lightPosition.z + 500,1) , glm::vec3(.25f,5.0f,.25f) ,glm::vec3(0),500, });

	auto directionalLight= m_sceneGraph.AddNode("DirectionalLight", Ignite::LightSceneObjectCreateInfo(Ignite::LightType::DIRECTIONAL));
	directionalLight->Transform.SetPosition(glm::vec3(0.4f, 1, 0.4f));

	auto pointLight = m_sceneGraph.AddNode("PointLight", Ignite::LightSceneObjectCreateInfo(Ignite::LightType::POINT, glm::vec3(2.0f,0.0f,0.0f)));
	pointLight->Transform.SetPosition(lightPosition);
	lights.push_back(pointLight);

	m_sceneGraph.AddNode("sponza", Ignite::ModelCreateInfo{ "sponza","resources/models/sponza", "sponza.obj" });

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
	lights[0]->Transform.SetPosition(lightPosition);
	////lights[2].Position.x = lightPosition.x;
	////lights[3].Position.x = lightPosition.x;

	constexpr float CAMERA_SPEED = 0.5f;
	//camera
	if (m_sceneGraph.GetMainCamera()) {
		if (Ignite::Input::IsKeyPressed(IG_KEY_W))
			m_sceneGraph.GetMainCamera()->Translate(Ignite::CameraDirection::eFORWARD, CAMERA_SPEED * deltaTime);
		if (Ignite::Input::IsKeyPressed(IG_KEY_S))
			m_sceneGraph.GetMainCamera()->Translate(Ignite::CameraDirection::eBACKWARD, CAMERA_SPEED * deltaTime);
		if (Ignite::Input::IsKeyPressed(IG_KEY_A))
			m_sceneGraph.GetMainCamera()->Translate(Ignite::CameraDirection::eLEFT, CAMERA_SPEED * deltaTime);
		if (Ignite::Input::IsKeyPressed(IG_KEY_D))
			m_sceneGraph.GetMainCamera()->Translate(Ignite::CameraDirection::eRIGHT, CAMERA_SPEED * deltaTime);

		m_sceneGraph.GetMainCamera()->MousePosition(Ignite::Input::GetMouseX(), Ignite::Input::GetMouseY());
	}
	
	m_sceneGraph.Render(effectTemplate);

	//FL_LOG_INFO("ExampleLayer::Update");
	if (Ignite::Input::IsKeyPressed(IG_KEY_ESCAPE))
	{
		Ignite::Application::Instance().Close();
	}
}

void SponzaTestLayer::OnEvent(Ignite::Event& event)
{
}
