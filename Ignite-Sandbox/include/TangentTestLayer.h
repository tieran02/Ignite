#pragma once
#include <memory>
#include "Ignite/Ignite.h"
#include <chrono>
#include "Ignite/Events/MouseEvent.h"
class TangentTestLayer : public Ignite::Layer
{
public:
	TangentTestLayer() : Layer("TangentTest") {}

	void OnAttach() override;

	void OnDetach() override;

	void OnUpdate() override;

	void OnEvent(Ignite::Event& event) override;

private:
	using ms = std::chrono::duration<float, std::milli>;
	
	const Ignite::Pipeline* pipeline;
	const Ignite::Pipeline* normalPipeline;
	const Ignite::Pipeline* normalMappingPipeline;
	const Ignite::Pipeline* debugNormalPipeline;

	std::shared_ptr<Ignite::Model> tangentModel;
	std::shared_ptr<Ignite::Model> cubeModel;
	std::vector<Ignite::LightData> lights;

	glm::vec3 lightPosition{ 50,100,-8 };
	Ignite::Camera camera{ glm::vec3(0,0,0) };

	std::chrono::high_resolution_clock::time_point lastPrintTime;
	std::chrono::high_resolution_clock::time_point lastFrameTime;
	int nbFrames = 0;
	float deltaTime = 16.0f;
};
