#pragma once
#include <Ignite/Transform.h>
#include "SceneObject.h"

namespace Ignite
{
	class Camera;

	struct SceneNode
	{
		//NOTE: Name needs to be unique for scene graph, possible have a GUID in the future.
		std::string Name;
		bool IsEnabled;
		Transform Transform;
		std::unique_ptr<SceneObject> Object;
		SceneNode* Parent;
	};

	class SceneGraph
	{
		public:
			
			void AddObject(const std::string& name, SceneObject& object, Transform transform = Transform{});

			void Render();
		private:
			std::unordered_map<std::string ,SceneNode> m_sceneNodes;
			Camera& m_mainCamera;
	};
}
