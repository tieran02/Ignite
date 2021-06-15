#include "igpch.h"
#include "Ignite/SceneObject.h"
#include "Ignite/Renderer/Camera.h"
#include "Ignite/Renderer/Light.h"

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

	std::unique_ptr<SceneObject> SceneObject::CreateObject(SceneObjectCreateInfo createInfo)
	{
		switch (createInfo.ObjectType())
		{
		case SceneObjectType::MESH:
			break;
		case SceneObjectType::MODEL:
			break;
		case SceneObjectType::LIGHT:
		{
			auto lightCreateInfo = reinterpret_cast<LightSceneObjectCreateInfo*>(&createInfo);
			return std::make_unique<Light>(lightCreateInfo->LightType, lightCreateInfo->ObjectTransfrom.Position(), lightCreateInfo->Color);
		}
		case SceneObjectType::CAMERA:
		{
			auto cameraCreateInfo = reinterpret_cast<CameraSceneObjectCreateInfo*>(&createInfo);
			return std::make_unique<Camera>(cameraCreateInfo->ObjectTransfrom.Position());
		}
		case SceneObjectType::UNDEFINED:
			break;
		}
	}
}
