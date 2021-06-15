#include "igpch.h"
#include "Ignite/SceneGraph.h"
#include <Ignite/Renderer/Renderer.h>
#include <Ignite/Renderer/RenderCommand.h>

namespace Ignite
{

	void SceneGraph::AddObject(const std::string& name, SceneObject& object, Transform transform)
	{
		//if (m_sceneNodes.find(name) != m_sceneNodes.end())
		//	return;

		//SceneNode sceneNode
		//{
		//	name,
		//	true,
		//	transform,
		//	object,
		//	nullptr
		//};

		//switch (sceneNode.Object.ObjectType())
		//{
		//default:
		//	break;
		//}

		//m_sceneNodes.insert(std::make_pair(name, sceneNode));
	}

	void SceneGraph::Render()
	{
		////start scene
		//RenderCommand::SetClearColor(glm::vec4{ .5f,.2f,.2f,1.0f });

		//Renderer::BeginScene(camera, lights);

		//Renderer::Submit(pipeline, sponzaModel.get(), glm::translate(glm::mat4(1), glm::vec3(0, 0, 0)));
		////Ignite::Renderer::Submit(geom, sponzaModel.get(), glm::translate(glm::mat4(1), glm::vec3(0, 0, 0)));

		////render light
		//Renderer::Submit(unlitPipeline, cubeModel.get(), glm::translate(glm::mat4(1), lightPosition));

		//Renderer::EndScene();

		//Renderer::SwapBuffers();
	}
}