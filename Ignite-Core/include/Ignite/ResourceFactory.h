#pragma once
#include <memory>
#include <type_traits>
#include "Ignite/IWindow.h"
#include "Ignite/Renderer/Buffer.h"
#include "Ignite/Renderer/Material.h"
#include "Ignite/Renderer/Mesh.h"
#include "Ignite/Renderer/Model.h"

namespace Ignite 
{
	template<typename> struct ProperitesMap;
	template<> struct ProperitesMap<IWindow>	{ using properties = WindowProperites;		inline static const	 std::string name{ "Window" }; };
	template<> struct ProperitesMap<Pipeline>	{ using properties = PipelineCreateInfo;	inline static const  std::string name{ "Pipeline" }; };
	template<> struct ProperitesMap<Buffer>		{ using properties = BufferCreateInfo;		inline static const  std::string name{ "Buffer" }; };
	template<> struct ProperitesMap<Material>	{ using properties = MaterialCreateInfo;	inline static const  std::string name{ "Material" }; };
	template<> struct ProperitesMap<Mesh>		{ using properties = MeshCreateInfo;		inline static const  std::string name{ "Mesh" }; };
	template<> struct ProperitesMap<Model>		{ using properties = ModelCreateInfo;		inline static const  std::string name{ "Model" }; };
	template<> struct ProperitesMap<Texture2D>	{ using properties = Texture2DCreateInfo;	inline static const  std::string name{ "Texture2D" }; };

	class ResourceFactory
	{
	public:
		template <typename T>
		inline static std::unique_ptr<T> Create(const typename ProperitesMap<T>::properties& createProperties)
		{
			return T::Create(createProperties);
		}

		template <typename T>
		inline static const T* Create(const typename ProperitesMap<T>::properties& createProperties, std::unordered_map<std::string, std::unique_ptr<T>>& map)
		{
			bool exists = map.find(createProperties.GetName()) != map.end();

			if (exists)
			{
				LOG_CORE_TRACE("{0}: {1} already exists within map", createProperties.GetName(), ProperitesMap<T>::name);
				return map[createProperties.GetName()].get();
			}

			auto inserted = map.insert(std::make_pair(createProperties.GetName(), std::move(ResourceFactory::Create<T>(createProperties))));
			return inserted.first->second.get();
		}
	};
}