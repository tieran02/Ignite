#pragma once
#include "Ignite/Ignite.h"
#include "platform/ImGUI/ImGUI.h"

class TestUILayer : public Ignite::Layer
{
public:
	TestUILayer() : Layer("TestUILayer") {}

	void OnAttach() override;

	void OnDetach() override;

	void OnUpdate() override;

	void OnEvent(Ignite::Event& event) override;

private:
	std::unique_ptr<ImGUI> imGUI;

	std::vector<Ignite::LightData> lights;
	Ignite::Camera camera{ glm::vec3(0,0,0) };
};
