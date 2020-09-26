#include "TestUILayer.h"

void TestUILayer::OnAttach()
{
	imGUI = ImGUI::Create();
	imGUI->Init(static_cast<float>(Ignite::Application::Instance().Window()->Width()),
		static_cast<float>(Ignite::Application::Instance().Window()->Height()));
}

void TestUILayer::OnDetach()
{
	imGUI.reset();
}

void TestUILayer::OnUpdate()
{
	//start scene
	Ignite::RenderCommand::SetClearColor(glm::vec4{ .5f,.2f,.2f,1.0f });

	Ignite::Renderer::BeginScene(camera, lights);
	
	imGUI->NewFrame(true);

	imGUI->UpdateBuffers();

	imGUI->DrawFrame();

	Ignite::Renderer::EndScene();

	Ignite::Renderer::SwapBuffers();

	if (Ignite::Input::IsKeyPressed(IG_KEY_ESCAPE))
	{
		Ignite::Application::Instance().Close();
	}
}

void TestUILayer::OnEvent(Ignite::Event& event)
{
}
