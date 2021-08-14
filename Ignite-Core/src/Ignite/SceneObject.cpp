#include "igpch.h"
#include "Ignite/SceneObject.h"
#include "Ignite/Renderer/Camera.h"
#include "Ignite/Renderer/Mesh.h"
#include "Ignite/Renderer/Light.h"
#include "Ignite/Renderer/Model.h"

namespace Ignite
{
	SceneObjectCreateInfo::SceneObjectCreateInfo(SceneObjectType objectType) : m_objectType(objectType)
	{
		
	}

	SceneObjectType SceneObjectCreateInfo::ObjectType() const
	{
		return m_objectType;
	}


	SceneObject::SceneObject(SceneObjectType objectType) : m_objectType(objectType)
	{
		
	}

	SceneObject::SceneObject(SceneObjectCreateInfo createInfo) : m_objectType(createInfo.ObjectType())
	{
	}

	SceneObjectType SceneObject::ObjectType() const
	{
		return m_objectType;
	}

	std::unique_ptr<SceneObject> SceneObject::CreateObject(const SceneObjectCreateInfo* createInfo)
	{
		switch (createInfo->ObjectType())
		{
		case SceneObjectType::MESH:
		{
			const auto meshCreateInfo = reinterpret_cast<const MeshCreateInfo*>(createInfo);
			return Mesh::Create(*meshCreateInfo);
		}
		case SceneObjectType::MODEL:
		{
			const auto modelCreateInfo = reinterpret_cast<const ModelCreateInfo*>(createInfo);
			return Model::Create(*modelCreateInfo);
		}
		case SceneObjectType::LIGHT:
		{
			const auto lightCreateInfo = reinterpret_cast<const LightSceneObjectCreateInfo*>(createInfo);
			return std::make_unique<Light>(lightCreateInfo->LightType, lightCreateInfo->ObjectTransfrom.Position(), lightCreateInfo->Color);
		}
		case SceneObjectType::CAMERA:
		{
			const auto cameraCreateInfo = reinterpret_cast<const CameraSceneObjectCreateInfo*>(createInfo);
			return std::make_unique<Camera>(cameraCreateInfo->ObjectTransfrom.Position());
		}
		case SceneObjectType::UNDEFINED:
			break;
		}
		return nullptr;
	}
}
