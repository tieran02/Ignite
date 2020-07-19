#pragma once
#include <glm/glm.hpp>
#include "Vertex.h"


namespace Ignite
{
	struct MeshData
	{
	public:

		MeshData() = default;
		MeshData(uint32_t vertexCount, uint32_t triangleCount);

		std::vector<glm::vec3>& Positions() { return m_positions; }
		std::vector<glm::vec3>& Normals() { return m_normals; }
		std::vector<glm::vec4>& Tangents() { return m_tangents; }
		std::vector<glm::vec3>& Bitangents() { return m_bitangents; }
		std::vector<glm::vec2>& TextureCoords() { return m_textureCoords; }
		std::vector<glm::uint32_t>& Triangles() { return m_triangles; }

		const std::vector<glm::vec3>& Positions() const { return m_positions; }
		const std::vector<glm::vec3>& Normals() const { return m_normals; }
		const std::vector<glm::vec4>& Tangents() const { return m_tangents; }
		const std::vector<glm::vec3>& Bitangents() const { return m_bitangents; }
		const std::vector<glm::vec2>& TextureCoords() const { return m_textureCoords; }
		const std::vector<glm::uint32_t>& Triangles() const { return m_triangles; }

		void CalculateTangents();
		bool HasData() const;

		std::vector<Vertex> ToVertices() const;

	private:
		std::vector<glm::vec3> m_positions;
		std::vector<glm::vec3> m_normals;
		std::vector<glm::vec4> m_tangents;
		std::vector<glm::vec3> m_bitangents;
		std::vector<glm::vec2> m_textureCoords;

		std::vector<uint32_t> m_triangles;
	};
}
