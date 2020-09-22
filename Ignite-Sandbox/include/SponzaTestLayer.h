#pragma once
#include "Ignite/Ignite.h"

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
	
	const Ignite::IPipeline* pipeline;
	const Ignite::IPipeline* unlitPipeline;

	std::shared_ptr<Ignite::Model> sponzaModel;
	std::shared_ptr<Ignite::Model> cubeModel;
	std::vector<Ignite::Light> lights;
	Ignite::Camera camera{ glm::vec3(0,0,0) };
	glm::vec3 lightPosition{ 50,100,-8 };

	std::chrono::high_resolution_clock::time_point lastPrintTime;
	std::chrono::high_resolution_clock::time_point lastFrameTime;
	int nbFrames = 0;
	float deltaTime = 16.0f;
};
