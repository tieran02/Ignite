#pragma once
#include "Vertex.h"

template <typename T, typename... Rest>
void hash_combine(std::size_t& seed, const T& v, const Rest&... rest)
{
	seed ^= std::hash<T>{}(v)+0x9e3779b9 + (seed << 6) + (seed >> 2);
	(hash_combine(seed, rest), ...);
}

namespace Ignite
{
	class ModelLoader
	{
	private:
		ModelLoader(const std::string& path);
	public:
		~ModelLoader();
		static std::unique_ptr<ModelLoader> Load(const std::string& path);

		const std::vector<Vertex>& Vertices() const { return vertices; }
		const std::vector<uint32_t>& Indices() const { return indices; }
	private:
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		void LoadModel(const std::string& path);
	};
}