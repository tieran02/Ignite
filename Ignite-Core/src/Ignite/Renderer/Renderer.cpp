#include "igpch.h"
#include "Ignite/Renderer/Renderer.h"
#include "Ignite/Renderer/RenderCommand.h"
#include "Ignite/Renderer/IPipeline.h"
#include "Ignite/Application.h"
#include "Ignite/Renderer/IModel.h"
#include "glm/gtx/associated_min_max.hpp"

bool Ignite::Renderer::m_recordingScene = false;

void Ignite::Renderer::Init()
{
	RenderCommand::Init();
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

void Ignite::Renderer::Submit(const IPipeline* pipeline, const IModel* model, const glm::mat4& transform)
{
	if (Application::Instance().Window()->Width() <= 0 || Application::Instance().Window()->Height() <= 0)
		return;
	
	//TODO sumbit vertex buffer aswell

	//bind pipeline
	pipeline->Bind();
	//TODO draw stuff here
	model->VertexBuffer()->Bind();
	model->IndexBuffer()->Bind();
	model->BindDescriptors();
	
	RenderCommand::DrawIndexed(model->VertexBuffer(), model->IndexBuffer(),model->IndexCount());
	
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
