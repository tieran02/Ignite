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
        glm::vec4 Tangent;
        //glm::vec3 Bitangent;
        glm::vec2 TexCoord;
    	
        bool operator==(const Vertex& other) const {
            return Position == other.Position && Normal == other.Normal && TexCoord == other.TexCoord;
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