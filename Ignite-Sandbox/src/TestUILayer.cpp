#include "TestUILayer.h"

void TestUILayer::OnAttach()
{
	imGUI = ImGUI::Create();
	imGUI->Init(static_cast<float>(Ignite::Application::Instance().Window()->Width()),
		static_cast<float>(Ignite::Application::Instance().Window()->Height()));
}

void TestUILayer::OnDetach()
{
}

void TestUILayer::OnUpdate()
{
}

void TestUILayer::OnEvent(Ignite::Event& event)
{
}
