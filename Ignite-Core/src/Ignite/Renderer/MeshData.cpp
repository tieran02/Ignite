#include "igpch.h"
#include "Ignite/Renderer/MeshData.h"


#include "mikktspace.h"
#include "glm/gtx/compatibility.hpp"

namespace Ignite
{
    static int mikk_get_num_faces(const SMikkTSpaceContext* context)
    {
        const MeshData* userdata = (const MeshData*)context->m_pUserData;
        return static_cast<int>(userdata->Triangles().size()) / 3;
    }

    static int mikk_get_num_verts_of_face(const SMikkTSpaceContext* context, const int face_num)
    {
        return 3;
    }

    static int mikk_vertex_index(const MeshData* mesh, const int face_num, const int vert_num)
    {
        return mesh->Triangles()[3 * face_num + vert_num];
    }

    static int mikk_corner_index(const MeshData* mesh, const int face_num, const int vert_num)
    {
         return mesh->Triangles()[3 * face_num + vert_num];
    }


    static void mikk_get_position(const SMikkTSpaceContext* context,
        float P[3],
        const int face_num,
        const int vert_num)
    {
        const MeshData* userdata = (const MeshData*)context->m_pUserData;
        const int vertex_index = mikk_vertex_index(userdata, face_num, vert_num);
        const glm::vec3 vP = userdata->Positions()[vertex_index];
        P[0] = vP.x;
        P[1] = vP.y;
        P[2] = vP.z;
    }

    static void mikk_get_texture_coordinate(const SMikkTSpaceContext* context,
        float uv[2],
        const int face_num,
        const int vert_num)
    {
        const MeshData* userdata = (const MeshData*)context->m_pUserData;
        if (!userdata->TextureCoords().empty()) {
            const int corner_index = mikk_corner_index(userdata, face_num, vert_num);
            glm::vec2 tfuv = userdata->TextureCoords()[corner_index];
            uv[0] = tfuv.x;
            uv[1] = tfuv.y;
        }
        else 
        {
            uv[0] = 0.0f;
            uv[1] = 0.0f;
        }
    }

    static void mikk_get_normal(const SMikkTSpaceContext* context,
        float N[3],
        const int face_num,
        const int vert_num)
    {
        const MeshData* userdata = (const MeshData*)context->m_pUserData;
        glm::vec3 vN;

        const int vertex_index = mikk_vertex_index(userdata, face_num, vert_num);
        vN = userdata->Normals()[vertex_index];

        N[0] = vN.x;
        N[1] = vN.y;
        N[2] = vN.z;
    }

    static void mikk_set_tangent_space(const SMikkTSpaceContext* context,
        const float T[],
        const float sign,
        const int face_num,
        const int vert_num)
    {
        MeshData* userdata = (MeshData*)context->m_pUserData;
        const int corner_index = mikk_corner_index(userdata, face_num, vert_num);

        glm::vec4 tangent = glm::vec4(T[0], T[1], T[2], sign);
        userdata->Tangents()[corner_index] = tangent;

        glm::vec3 normal = userdata->Normals()[corner_index];

        userdata->Bitangents()[corner_index] = sign * glm::cross(normal, glm::vec3(tangent));

        /*if (userdata->tangent_sign != NULL) {
            userdata->tangent_sign[corner_index] = sign;
        }*/
    }

    MeshData::MeshData(uint32_t vertexCount, uint32_t triangleCount)
    {
        m_positions = std::vector<glm::vec3>(vertexCount);
        m_normals = std::vector<glm::vec3>(vertexCount);
        m_tangents = std::vector<glm::vec4>(vertexCount);
        m_bitangents = std::vector<glm::vec3>(vertexCount);
        m_textureCoords = std::vector<glm::vec2>(vertexCount);
        m_triangles = std::vector<uint32_t>(triangleCount);
    }

	void MeshData::CalculateTangents()
	{
        m_tangents = std::vector<glm::vec4>(m_positions.size());
        m_bitangents = std::vector<glm::vec3>(m_positions.size());

        /* Setup interface. */
        SMikkTSpaceInterface sm_interface;
        memset(&sm_interface, 0, sizeof(sm_interface));
        sm_interface.m_getNumFaces = mikk_get_num_faces;
        sm_interface.m_getNumVerticesOfFace = mikk_get_num_verts_of_face;
        sm_interface.m_getPosition = mikk_get_position;
        sm_interface.m_getTexCoord = mikk_get_texture_coordinate;
        sm_interface.m_getNormal = mikk_get_normal;
        sm_interface.m_setTSpaceBasic = mikk_set_tangent_space;

        /* Setup context. */
        SMikkTSpaceContext context;
        memset(&context, 0, sizeof(context));
        context.m_pUserData = this;
        context.m_pInterface = &sm_interface;
        /* Compute tangents. */
        genTangSpaceDefault(&context);
	}

    bool MeshData::HasData() const
    {
        return !m_positions.empty() && !m_triangles.empty();
    }

    std::vector<Vertex> MeshData::ToVertices() const
    {
        std::vector<Vertex> vertices(m_positions.size());

        for (int i = 0; i < m_positions.size(); ++i)
        {
            Vertex vertex;

            vertex.Position = m_positions[i];
            vertex.Normal = m_normals[i];
            vertex.Tangent = m_tangents[i];
            //vertex.Bitangent = m_bitangents[i];
            vertex.TexCoord = m_textureCoords[i];

            vertices[i] = vertex;
        }
        return vertices;
    }
}
