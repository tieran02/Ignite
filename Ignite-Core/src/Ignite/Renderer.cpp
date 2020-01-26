#include "igpch.h"
#include "Ignite/Renderer.h"
#include "Ignite/RenderCommand.h"

void Ignite::Renderer::Init()
{
	RenderCommand::Init();
}

void Ignite::Renderer::Shutdown()
{
	RenderCommand::s_renderer = nullptr;
}

bool Ignite::Renderer::IsInitialised()
{
	return RenderCommand::s_renderer != nullptr;
}
