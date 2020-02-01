#include "Ignite/Application.h"
#include <memory>
#include "Ignite/Input.h"
#include "Ignite/Renderer/IPipeline.h"
#include "Ignite/Renderer/RenderCommand.h"
#include "Ignite/Renderer/Renderer.h"
#include "Ignite/Ignite.h"

class ExampleLayer : public Ignite::Layer
{
public:
	ExampleLayer() : Layer("Example") {}

	void OnAttach() override
	{
		pipeline = Ignite::IPipeline::Create("resources/shaders/vert.spv", "resources/shaders/frag.spv");
	}

	void OnDetach() override
	{
		pipeline.reset();
	}

	void OnUpdate() override
	{
        //start scene
        Ignite::RenderCommand::SetClearColor(glm::vec4{ .5f,.2f,.2f,1.0f });

        Ignite::Renderer::BeginScene();

        Ignite::Renderer::Submit(pipeline.get());

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
		//FL_LOG_TRACE("{0}", event);
	}

private:
	std::unique_ptr<Ignite::IPipeline> pipeline;
};

int main()
{
	Ignite::Ignite ignite;
	ignite.App().PushLayer(new ExampleLayer);
	
    ignite.App().Start(1920, 1080);

    return 0;
}
