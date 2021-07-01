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
		LightSceneObjectCreateInfo(LightType type, const glm::vec3 color = glm::vec3(1.0f)) : SceneObjectCreateInfo(SceneObjectType::LIGHT), LightType(type), Color(color)
		{
		}

		LightType LightType;
		glm::vec3 Color;
	};
	
	struct CameraSceneObjectCreateInfo : SceneObjectCreateInfo
	{
		CameraSceneObjectCreateInfo(float fov, float  nearClip, float farClip) : SceneObjectCreateInfo(
			SceneObjectType::CAMERA), FOV(fov), NearClip(nearClip), FarClip(farClip)
		{
		}

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

		static std::unique_ptr<SceneObject> CreateObject(const SceneObjectCreateInfo* createInfo);
	private:
		SceneObjectType m_objectType;
	};
}
