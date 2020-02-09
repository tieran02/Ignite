#include <memory>
#include "Ignite/Ignite.h"

class ExampleLayer : public Ignite::Layer
{
public:
	ExampleLayer() : Layer("Example") {}

	void OnAttach() override
	{
		Ignite::PipelineInputLayout layout = 
		{
			{ Ignite::PipelineDataType::eFloat2, "a_Position" },
			{ Ignite::PipelineDataType::eFloat3, "a_Color" }
		};
		
		pipeline = Ignite::IPipeline::Create("shader","resources/shaders/vert.spv", "resources/shaders/frag.spv", layout);

		
		vertexBuffer = Ignite::IVertexBuffer::Create(vertices.data(), sizeof(float) * vertices.size());
		indexBuffer = Ignite::IIndexBuffer::Create(indices.data(), sizeof(uint16_t) * indices.size());
	}

	void OnDetach() override
	{
	}

	void OnUpdate() override
	{
        //start scene
        Ignite::RenderCommand::SetClearColor(glm::vec4{ .5f,.2f,.2f,1.0f });

        Ignite::Renderer::BeginScene();

        Ignite::Renderer::Submit(pipeline.get(), vertexBuffer.get(), indexBuffer.get(), indices.size());

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
	std::shared_ptr<Ignite::IPipeline> pipeline;

	//two floats pos, three floats color
    std::vector<float> vertices = 
	{
	-0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
	0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
	0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
	-0.5f, 0.5f, 1.0f, 1.0f, 1.0f
	};

	std::vector<uint16_t> indices = 
	{
		0, 1, 2,
		2, 3, 0
	};
	
	std::shared_ptr<Ignite::IVertexBuffer> vertexBuffer;
	std::shared_ptr<Ignite::IIndexBuffer> indexBuffer;
};

int main()
{
	Ignite::Ignite ignite;
	ignite.App().PushLayer(new ExampleLayer);
	
    ignite.App().Start(1920, 1080);

    return 0;
}
