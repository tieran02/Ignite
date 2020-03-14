#include "igpch.h"
#include "Ignite/Renderer/Renderer.h"
#include "Ignite/Renderer/RenderCommand.h"
#include "Ignite/Renderer/IPipeline.h"
#include "Ignite/Application.h"
#include "Ignite/Renderer/IMesh.h"
#include "Ignite/Renderer/Model.h"
#include "Ignite/Renderer/IMaterial.h"
#include "Ignite/Renderer/Camera.h"
#include "glm/gtx/associated_min_max.hpp"

bool Ignite::Renderer::m_recordingScene = false;
Ignite::SceneUniformBuffer Ignite::Renderer::m_sceneUBO;

void Ignite::Renderer::Init()
{
	RenderCommand::Init();


	//add default textures
	std::shared_ptr<ITexture2D> defaultDiffuseTexture = ITexture2D::Create("default_diffuse", "resources/textures/default_white.jpg", TextureType::eDIFFUSE);
	std::shared_ptr<ITexture2D> defaultSpecularTexture = ITexture2D::Create("default_specular", "resources/textures/default_black.jpg", TextureType::eSPECULAR);
	std::shared_ptr<ITexture2D> defaultNormalTexture = ITexture2D::Create("default_normal", "resources/textures/default_normal.jpg", TextureType::eNORMAL);
}

void Ignite::Renderer::Shutdown()
{
	RenderCommand::s_renderer.reset();
}

bool Ignite::Renderer::IsInitialised()
{
	return RenderCommand::s_renderer != nullptr;
}

void Ignite::Renderer::BeginScene(const Camera& camera)
{
	if (Application::Instance().Window()->Width() <= 0 || Application::Instance().Window()->Height() <= 0)
		return;

	m_sceneUBO.proj = glm::perspective(glm::radians(75.0f), (float)Ignite::Application::Instance().Window()->Width() / (float)Ignite::Application::Instance().Window()->Height(), 0.1f, 5000.0f);
	m_sceneUBO.proj[1][1] *= -1;

	m_sceneUBO.view = camera.GetViewMatrix();
	m_sceneUBO.view_pos = camera.Position();
	
	m_recordingScene = true;
	//get the renderer api
	RenderCommand::s_renderer->BeginScene(camera);
}

void Ignite::Renderer::EndScene()
{
	if (Application::Instance().Window()->Width() <= 0 || Application::Instance().Window()->Height() <= 0)
		return;
	
	m_recordingScene = false;
	RenderCommand::s_renderer->EndScene();
}

void Ignite::Renderer::Submit(const IPipeline* pipeline, const IMesh* mesh,const glm::mat4& transform)
{
	if (Application::Instance().Window()->Width() <= 0 || Application::Instance().Window()->Height() <= 0)
		return;
	
	//bind pipeline
	pipeline->Bind();
		submitMesh(pipeline,mesh,transform);
	pipeline->Unbind();
}

void Ignite::Renderer::Submit(const IPipeline* pipeline, const Model* model, const glm::mat4& transform)
{
	if(!model)
		return;
	
	//bind pipeline
	pipeline->Bind();
		for (const std::shared_ptr<IMesh>& mesh : model->Meshes())
		{
			submitMesh(pipeline,mesh.get(),transform);
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

Ignite::IGraphicsContext* Ignite::Renderer::GraphicsContext()
{
	return RenderCommand::s_renderer->GetGraphicsContext();
}

Ignite::SceneUniformBuffer& Ignite::Renderer::SceneUBO()
{
	return m_sceneUBO;
}

void Ignite::Renderer::submitMesh(const IPipeline* pipeline, const IMesh* mesh, const glm::mat4& transform)
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
