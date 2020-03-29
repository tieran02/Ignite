#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include "glm/glm.hpp"

namespace Ignite
{
    struct Vertex
	{
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec3 Tangent;
        glm::vec3 Bitangent;
        glm::vec2 TexCoord;
    	
        bool operator==(const Vertex& other) const {
            return Position == other.Position && Normal == other.Normal && TexCoord == other.TexCoord;
        }

    	void CheckTangent()
        {
            // Calculate handedness
            if (glm::dot(glm::cross(Normal, Tangent), Bitangent) < 0.0f) {
                Tangent = Tangent * -1.0f;
            }

        }
    };
}

namespace std {
    template<> struct hash<Ignite::Vertex> {
        size_t operator()(Ignite::Vertex const& vertex) const {
            return ((hash<glm::vec3>()(vertex.Position) ^ (hash<glm::vec3>()(vertex.Normal) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.TexCoord) << 1);
        }
    };
}