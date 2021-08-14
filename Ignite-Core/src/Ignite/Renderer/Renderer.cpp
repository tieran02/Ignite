#include "igpch.h"
#include "Ignite/Renderer/Renderer.h"
#include "Ignite/Renderer/RenderCommand.h"
#include "Ignite/Renderer/Pipeline.h"
#include "Ignite/Application.h"
#include "Ignite/Renderer/Mesh.h"
#include "Ignite/Renderer/Model.h"
#include "Ignite/Renderer/Material.h"
#include "Ignite/Renderer/Camera.h"
#include "glm/gtx/associated_min_max.hpp"

bool Ignite::Renderer::m_recordingScene = false;
Ignite::SceneUniformBuffer Ignite::Renderer::m_sceneUBO;

void Ignite::Renderer::Init()
{
	RenderCommand::Init();


	//add default textures
	Texture2DCreateInfo defaultDiffuseInfo{ "default_diffuse", "resources/textures/default_white.jpg", TextureType::eDIFFUSE };
	Texture2DCreateInfo defaultSpecularInfo{ "default_specular", "resources/textures/default_black.jpg", TextureType::eSPECULAR };
	Texture2DCreateInfo defaultNormalInfo{ "default_normal", "resources/textures/default_normal.jpg", TextureType::eNORMAL };
	
	const Texture2D* defaultDiffuseTexture = GraphicsContext()->CreateTexture2D(defaultDiffuseInfo);
	const Texture2D* defaultSpecularTexture = GraphicsContext()->CreateTexture2D(defaultSpecularInfo);
	const Texture2D* defaultNormalTexture = GraphicsContext()->CreateTexture2D(defaultNormalInfo);
}

void Ignite::Renderer::Shutdown()
{
	RenderCommand::s_renderer.reset();
}

bool Ignite::Renderer::IsInitialised()
{
	return RenderCommand::s_renderer != nullptr;
}

void Ignite::Renderer::BeginScene(const Camera& camera, const std::vector<LightData>& lights)
{
	if (Application::Instance().Window()->Width() <= 0 || Application::Instance().Window()->Height() <= 0)
		return;

	m_sceneUBO.proj = glm::perspective(glm::radians(75.0f), (float)Ignite::Application::Instance().Window()->Width() / (float)Ignite::Application::Instance().Window()->Height(), 0.1f, 5000.0f);
	m_sceneUBO.proj[1][1] *= -1;

	m_sceneUBO.view = camera.GetViewMatrix();
	m_sceneUBO.view_pos = camera.Position();
	
	m_recordingScene = true;
	//get the renderer api
	RenderCommand::s_renderer->BeginScene(camera,lights);
}

void Ignite::Renderer::EndScene()
{
	if (Application::Instance().Window()->Width() <= 0 || Application::Instance().Window()->Height() <= 0)
		return;
	
	m_recordingScene = false;
	RenderCommand::s_renderer->EndScene();
}

void Ignite::Renderer::Submit(const Pipeline* pipeline, const Mesh* mesh,const glm::mat4& transform)
{
	if (Application::Instance().Window()->Width() <= 0 || Application::Instance().Window()->Height() <= 0)
		return;
	
	//bind pipeline
	pipeline->Bind();
		submitMesh(pipeline,mesh,transform);
	pipeline->Unbind();
}

void Ignite::Renderer::Submit(const Pipeline* pipeline, const Model* model, const glm::mat4& transform)
{
	if(!model)
		return;
	
	//bind pipeline
	pipeline->Bind();
		for (const Mesh* mesh : model->Meshes())
		{
			submitMesh(pipeline,mesh,transform);
		}
	pipeline->Unbind();
}

void Ignite::Renderer::SwapBuffers()
{
	if(Application::Instance().Window()->Width() <= 0 || Application::Instance().Window()->Height() <= 0)
		return;
	
	//swap buffers
	GraphicsContext()->SwapBuffers();
}

Ignite::GraphicsContext* Ignite::Renderer::GraphicsContext()
{
	return RenderCommand::s_renderer->GetGraphicsContext();
}

Ignite::SceneUniformBuffer& Ignite::Renderer::SceneUBO()
{
	return m_sceneUBO;
}

void Ignite::Renderer::submitMesh(const Pipeline* pipeline, const Mesh* mesh, const glm::mat4& transform)
{
	if (mesh != nullptr)
	{
		mesh->Material()->Bind(pipeline);
		mesh->VertexBuffer()->Bind();
		mesh->IndexBuffer()->Bind();
		//mesh->BindDescriptors();
		
		RenderCommand::DrawIndexed(mesh->VertexBuffer(), mesh->IndexBuffer(), mesh->IndexCount(),transform);
		mesh->Material()->Unbind(pipeline);
	}
}
