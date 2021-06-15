#pragma once
#include "Transform.h"


namespace Ignite
{
	enum class LightType;

	enum class SceneObjectType
	{
		MESH,
		MODEL,
		LIGHT,
		CAMERA,
		UNDEFINED
	};

	struct SceneObjectCreateInfo
	{
	protected:
		SceneObjectCreateInfo(SceneObjectType objectType);
	public:
		SceneObjectType ObjectType() const;
		Transform ObjectTransfrom;
	private:
		SceneObjectType m_objectType;
	};

	struct LightSceneObjectCreateInfo : SceneObjectCreateInfo
	{	
		LightSceneObjectCreateInfo() : SceneObjectCreateInfo(SceneObjectType::LIGHT) {}
		LightType LightType;
		glm::vec3 Color;
	};
	
	struct CameraSceneObjectCreateInfo : SceneObjectCreateInfo
	{
		CameraSceneObjectCreateInfo() : SceneObjectCreateInfo(SceneObjectType::CAMERA) {}
		float FOV;
		float NearClip;
		float FarClip;
	};

	class SceneObject
	{
	protected:
		SceneObject(SceneObjectType objectType);
	public:
		SceneObject(SceneObjectCreateInfo createInfo);
		SceneObjectType ObjectType() const;

		static std::unique_ptr<SceneObject> CreateObject(SceneObjectCreateInfo createInfo);
	private:
		SceneObjectType m_objectType;
	};
}
