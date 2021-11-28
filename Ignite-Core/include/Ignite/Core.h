#pragma once

#ifdef IG_APP
#include "igpch.h"
#endif // IGAPP


template <typename E>
constexpr auto to_underlying(E e) noexcept
{
	return static_cast<std::underlying_type_t<E>>(e);
}

namespace Ignite 
{
	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T, typename... Args>
	inline Scope<T> CreateScope(Args&&... args)
	{
		return Scope<T>(new T(std::forward<Args>(args)...));
	}

	template<typename T, typename... Args>
	inline Ref<T> CreateRef(Args&&... args)
	{
		return Ref<T>(new T(std::forward<Args>(args)...));
	}
}

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Ignite/Application.h"
#include "Ignite/Input.h"
#include "Ignite/UUID.h"
#include "Ignite/Renderer/Camera.h"
#include "Ignite/Renderer/Buffer.h"
#include "Ignite/Renderer/Mesh.h"
#include "Ignite/Renderer/Pipeline.h"
#include "Ignite/Renderer/Texture2D.h"
#include "Ignite/Renderer/Light.h"
#include "Ignite/Renderer/Model.h"
#include "Ignite/Renderer/PipelineInputLayout.h"
#include "Ignite/Renderer/RenderCommand.h"
#include "Ignite/Renderer/Renderer.h"
#include "Ignite/Renderer/DescriptorSet.h"
#include "Ignite/Renderer/MaterialSystem.h"
#include "Ignite/SceneGraph.h"