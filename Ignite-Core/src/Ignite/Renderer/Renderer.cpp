#include "igpch.h"
#include "Ignite/Renderer/Renderer.h"
#include "Ignite/Renderer/RenderCommand.h"
#include "Ignite/Renderer/IPipeline.h"
#include "Ignite/Application.h"
#include "Ignite/Renderer/IMesh.h"
#include "Ignite/Renderer/Model.h"
#include "glm/gtx/associated_min_max.hpp"

bool Ignite::Renderer::m_recordingScene = false;

void Ignite::Renderer::Init()
{
	RenderCommand::Init();


	//add default textures
	std::shared_ptr<ITexture2D> defaultTexture = ITexture2D::Create("default_white", "resources/textures/default_white.jpg", TextureType::eDIFFUSE);
}

void Ignite::Renderer::Shutdown()
{
	RenderCommand::s_renderer.reset();
}

bool Ignite::Renderer::IsInitialised()
{
	return RenderCommand::s_renderer != nullptr;
}

void Ignite::Renderer::BeginScene()
{
	if (Application::Instance().Window()->Width() <= 0 || Application::Instance().Window()->Height() <= 0)
		return;
	
	m_recordingScene = true;
	//get the renderer api
	RenderCommand::s_renderer->BeginScene();
}

void Ignite::Renderer::EndScene()
{
	if (Application::Instance().Window()->Width() <= 0 || Application::Instance().Window()->Height() <= 0)
		return;
	
	m_recordingScene = false;
	RenderCommand::s_renderer->EndScene();
}

void Ignite::Renderer::Submit(const IPipeline* pipeline, const IMesh* mesh, const glm::mat4& transform)
{
	if (Application::Instance().Window()->Width() <= 0 || Application::Instance().Window()->Height() <= 0)
		return;
	
	//bind pipeline
	pipeline->Bind();
	//TODO draw stuff here
	submitMesh(mesh);
	pipeline->Unbind();
}

void Ignite::Renderer::Submit(const IPipeline* pipeline, const Model* model, const glm::mat4& transform)
{
	if(!model)
		return;
	
	//bind pipeline
	pipeline->Bind();
	//TODO draw stuff here

	for (const std::shared_ptr<IMesh>& mesh : model->Meshes())
	{
		submitMesh(mesh.get());
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

void Ignite::Renderer::submitMesh(const IMesh* mesh, const glm::mat4& transform)
{
	if (mesh != nullptr)
	{
		mesh->VertexBuffer()->Bind();
		mesh->IndexBuffer()->Bind();
		mesh->BindDescriptors();

		RenderCommand::DrawIndexed(mesh->VertexBuffer(), mesh->IndexBuffer(), mesh->IndexCount());
	}
}
