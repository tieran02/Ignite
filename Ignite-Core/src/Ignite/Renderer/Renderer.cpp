#include "igpch.h"
#include "Ignite/Renderer/Renderer.h"
#include "Ignite/Renderer/RenderCommand.h"
#include "Ignite/Renderer/IPipeline.h"

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
	m_recordingScene = true;
	//get the renderer api
	RenderCommand::s_renderer->BeginScene();
}

void Ignite::Renderer::EndScene()
{
	m_recordingScene = false;
	RenderCommand::s_renderer->EndScene();
}

void Ignite::Renderer::Submit(const IPipeline* pipeline)
{
	//TODO sumbit vertex buffer aswell

	//bind pipeline
	pipeline->Bind();
	//TODO draw stuff here
	
	pipeline->Unbind();
	
}

void Ignite::Renderer::SwapBuffers()
{
	//swap buffers
	GraphicsContext()->SwapBuffers();
}

Ignite::IGraphicsContext* Ignite::Renderer::GraphicsContext()
{
	return RenderCommand::s_renderer->GetGraphicsContext();
}
