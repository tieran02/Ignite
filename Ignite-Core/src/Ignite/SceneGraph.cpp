#include "igpch.h"
#include "Ignite/SceneGraph.h"
#include <Ignite/Renderer/Renderer.h>
#include <Ignite/Renderer/RenderCommand.h>
#include <Ignite/Renderer/Pipeline.h>

#include "Ignite/Log.h"

namespace Ignite
{
	SceneGraph::SceneGraph()
	{
		CameraSceneObjectCreateInfo cameraCreateInfo(65.0f,0.01f, 1000.0f);
		const SceneNode* camera = AddNode("MainCamera", cameraCreateInfo);

		if (!camera)
		{
			LOG_CORE_FATAL("Failed to create main camera for scene graph");
		}

		m_mainCamera = reinterpret_cast<Camera*>(camera->Object.get());
	}

	SceneNode* SceneGraph::AddNode(const std::string& name, const SceneObjectCreateInfo& createInfo, Transform transform)
	{
		if (m_sceneNodes.find(name) != m_sceneNodes.end())
			return nullptr;

		if (!m_sceneNodes.insert(std::make_pair(name, 
			SceneNode
			{
				name,
				true,
				transform,
				SceneObject::CreateObject(&createInfo),
				nullptr
			})).second)
			return nullptr;

		//If light, Subscribe to the transform changed to update the light data that gets sent to the GPU
		if(createInfo.ObjectType() == SceneObjectType::LIGHT)
		{
			auto& node = m_sceneNodes[name];
			node.Transform.TransformChangedEventHandler().Subscribe("LightPosition", [&node](ValueChangedEvent<Transform>& event)
			{
				Light* light = reinterpret_cast<Light*>(node.Object.get());
				light->GetLightData().Position = glm::vec4(node.Transform.Position(), light->GetLightData().Position.w);
			});
		}
		
		return &m_sceneNodes[name];
	}

	Camera* SceneGraph::GetMainCamera() const
	{
		return m_mainCamera;
	}

	void SceneGraph::Render(Ref<EffectTemplate> effectTemplate)
	{
		//get light data, this copied for now (needs improving later)
		std::vector<LightData> lightData;
		
		std::vector<Model*> modelData;
		
		for (const auto& node : m_sceneNodes)
		{
			switch (node.second.Object->ObjectType())
			{
			case SceneObjectType::MESH: break;
			case SceneObjectType::MODEL:
			{
				Model* model = reinterpret_cast<Model*>(node.second.Object.get());
				modelData.push_back(model);
				break;
			}
			case SceneObjectType::LIGHT:
			{
				Light* light = reinterpret_cast<Light*>(node.second.Object.get());	
				lightData.push_back(light->GetLightData());
				break;
			}
			case SceneObjectType::CAMERA: break;
			case SceneObjectType::UNDEFINED: break;
			default: ;
			}
		}
		
		//start scene
		RenderCommand::SetClearColor(glm::vec4{ .5f,.2f,.2f,1.0f });

		Renderer::BeginScene(*m_mainCamera, lightData);

		//Render the effect template
		//Loop through all passes and render to a frame buffer 
		for (auto& renderPass : effectTemplate->PassShaders())
		{
			if (!renderPass || !renderPass->GetPipeline()) continue;

			for (const auto& model : modelData)
			{
				Renderer::Submit(renderPass->GetPipeline(), model, glm::translate(glm::mat4(1), glm::vec3(0, 0, 0)));
			}

		}


		
		//Renderer::Submit(pipeline, sponzaModel.get(), glm::translate(glm::mat4(1), glm::vec3(0, 0, 0)));
		////Ignite::Renderer::Submit(geom, sponzaModel.get(), glm::translate(glm::mat4(1), glm::vec3(0, 0, 0)));

		////render light
		//Renderer::Submit(unlitPipeline, cubeModel.get(), glm::translate(glm::mat4(1), lightPosition));

		Renderer::EndScene();

		Renderer::SwapBuffers();
	}
}
