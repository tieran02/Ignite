#include <memory>
#include "Ignite/Ignite.h"


#include <chrono>
#include "Ignite/Renderer/IModel.h"


class ExampleLayer : public Ignite::Layer
{
public:
	ExampleLayer() : Layer("Example") {}

	void OnAttach() override
	{
		Ignite::PipelineInputLayout layout = 
		{
			{ Ignite::PipelineDataType::eFloat2, "a_Position" },
			{ Ignite::PipelineDataType::eFloat3, "a_Color" },
			{ Ignite::PipelineDataType::eFloat2, "a_TexCoord" }
		};
		
		pipeline = Ignite::IPipeline::Create("shader","resources/shaders/vert.spv", "resources/shaders/frag.spv", layout);

		//create texture
		std::shared_ptr<Ignite::ITexture2D> image = Ignite::ITexture2D::Create("texture", "resources/textures/texture.jpg");

		//two floats pos, three floats color
		std::vector<float> vertices =
		{
			-0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
			0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
			0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
			-0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f
		};

		std::vector<uint16_t> indices =
		{
			0, 1, 2,
			2, 3, 0
		};

		model = Ignite::IModel::Create(vertices, indices, "texture");

		m_ubo.view = glm::lookAt(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		m_ubo.proj = glm::perspective(glm::radians(45.0f), (float)Ignite::Application::Instance().Window()->Width() / (float)Ignite::Application::Instance().Window()->Height(), 0.1f, 10.0f);
		m_ubo.proj[1][1] *= -1;
		m_ubo.model = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	}

	void OnDetach() override
	{
	}

	void OnUpdate() override
	{
		//rotate
		static auto startTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
		m_ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		
        //start scene
        Ignite::RenderCommand::SetClearColor(glm::vec4{ .5f,.2f,.2f,1.0f });

		Ignite::RenderCommand::SetUniformBufferObject(m_ubo);
		
        Ignite::Renderer::BeginScene();

        Ignite::Renderer::Submit(pipeline.get(), model.get());

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
	
	std::shared_ptr<Ignite::IModel> model;
	Ignite::UniformBufferObject m_ubo;
};

int main()
{
	Ignite::Ignite ignite;
	ignite.App().PushLayer(new ExampleLayer);
	
    ignite.App().Start(1920, 1080);

    return 0;
}
