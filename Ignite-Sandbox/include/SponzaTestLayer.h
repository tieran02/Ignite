#pragma once
#include "Ignite/Core.h"


class SponzaTestLayer : public Ignite::Layer
{
public:
	SponzaTestLayer() : Layer("SponzaTest") {}

	void OnAttach() override;

	void OnDetach() override;

	void OnUpdate() override;

	void OnEvent(Ignite::Event& event) override;

private:
	using ms = std::chrono::duration<float, std::milli>;
	
	//const Ignite::Pipeline* pipeline{ nullptr };
	//const Ignite::Pipeline* geom{ nullptr };
	//const Ignite::Pipeline* unlitPipeline{ nullptr };

	Ignite::Ref<Ignite::DescriptorSetLayout> sceneLayout;
	Ignite::Ref<Ignite::DescriptorSetLayout> textureLayout;
	Ignite::Ref<Ignite::DescriptorSetLayout> lightLayout;

	Ignite::Ref<Ignite::ShaderEffect> defaultShaderEffect;
	Ignite::Ref<Ignite::ShaderPass> defaultPass;
	Ignite::Ref<Ignite::EffectTemplate> effectTemplate;

	std::shared_ptr<Ignite::Model> sponzaModel;
	std::shared_ptr<Ignite::Model> cubeModel;
	std::vector<Ignite::SceneNode*> lights;
	Ignite::Camera camera{ glm::vec3(0,0,0) };
	glm::vec3 lightPosition{ 50,100,-8 };

	Ignite::SceneGraph m_sceneGraph;

	std::chrono::high_resolution_clock::time_point lastPrintTime;
	std::chrono::high_resolution_clock::time_point lastFrameTime;
	int nbFrames = 0;
	float deltaTime = 16.0f;
};
