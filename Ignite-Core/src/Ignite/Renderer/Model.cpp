#include "igpch.h"
#include "Ignite/Renderer/Model.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "Ignite/Log.h"
#include "Ignite/Renderer/Renderer.h"
#include "glm/gtc/type_ptr.inl"
#include "mikktspace.h"
#include "Ignite/Renderer/MeshData.h"

namespace Ignite
{
	std::unique_ptr<Model> Model::Load(const std::string& path, const std::string& file)
	{
        std::unique_ptr<Model> model = std::unique_ptr<Model>(new Model);
        model->loadModel(path, file);
		if(!model->m_meshes.empty())
			return model;

        return std::unique_ptr<Model>();
	}
	
	const std::vector<std::shared_ptr<IMesh>>& Model::Meshes() const
	{
		return m_meshes;
	}

	const std::vector<std::shared_ptr<IMaterial>>& Model::Materials() const
	{
        return m_materials;
	}

    void Model::loadModel(const std::string& path, const std::string& file)
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        std::string fullPath = path + "/" + file;
        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, fullPath.c_str(), path.c_str()))
        {
            CORE_ASSERT(false, warn + err);
        }

        for (const auto& shape : shapes)
        {
			m_meshes.push_back(processMesh(attrib, shape, materials, path));
        }
    }

    std::shared_ptr<IMesh> Model::processMesh(const tinyobj::attrib_t& attrib, const tinyobj::shape_t& shape, const std::vector<tinyobj::material_t>& materials, const std::string& path)
    {	
        std::vector<uint32_t> indices;
        std::vector<std::shared_ptr<ITexture2D>> textures;

        //MeshData meshData(attrib.vertices.size(),shape.mesh.indices.size());
        MeshData meshData;
        Vertex vertex{};

        std::unordered_map<Vertex, uint32_t> uniqueVertices{};

        for (const auto& index : shape.mesh.indices)
        {
            glm::vec3 positon = {
                    attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
            };
            vertex.Position = positon;

            if (!attrib.normals.empty()) // does the mesh contain texture coordinates?
            {
                glm::vec3 normal = {
					attrib.normals[3 * index.normal_index + 0],
					attrib.normals[3 * index.normal_index + 1],
					attrib.normals[3 * index.normal_index + 2]
                };

                vertex.Normal = normal;
            }
            else
            {
                glm::vec3 normal = { 0.0f,1.0f,0.0f };
                vertex.Normal = normal;
            }

            if (!attrib.texcoords.empty()) // does the mesh contain texture coordinates?
            {
                glm::vec2 texCoord =
                {
				attrib.texcoords[2 * index.texcoord_index + 0],
				attrib.texcoords[2 * index.texcoord_index + 1]
                };

                vertex.TexCoord = texCoord;
            }
            else
            {
                glm::vec2 texCoord = { 0.0f,0.0f };

                vertex.TexCoord = texCoord;
            }

            if (uniqueVertices.count(vertex) == 0)
            {
                uniqueVertices[vertex] = static_cast<uint32_t>(meshData.Positions().size());
                meshData.Positions().push_back(vertex.Position);
                meshData.Normals().push_back(vertex.Normal);
                meshData.TextureCoords().push_back(vertex.TexCoord);
            }



            meshData.Triangles().push_back(uniqueVertices[vertex]);
        }


        std::shared_ptr<IMaterial> material;

        // Vulkan viewer does not support texturing with per-face material.
        if (!shape.mesh.material_ids.empty() && shape.mesh.material_ids[0] >= 0)
        {
            material = getMaterial(materials, shape.mesh.material_ids[0], path);
        }else
        {
            //make default mat
            material = IMaterial::Create("default", nullptr);
        }
        m_materials.push_back(material);
        //computeTangentBasis(indices, vertices);
		//finaly create mesh

        meshData.CalculateTangents();

        return IMesh::Create(meshData, material);
	}

	std::shared_ptr<IMaterial> Model::getMaterial(const std::vector<tinyobj::material_t>& materials, int materialID,
		const std::string& path)
	{
        if(materialID < materials.size())
        {
            const tinyobj::material_t* mp = &materials[materialID];

            std::shared_ptr<ITexture2D> diffuseTexture;
            if (mp->diffuse_texname.length() > 0)
			{
			   std::string file = path + "/" + mp->diffuse_texname;
			   diffuseTexture = ITexture2D::Create(mp->diffuse_texname, file, TextureType::eDIFFUSE);
			}

            std::shared_ptr<ITexture2D> specularTexture;
            if (mp->specular_texname.length() > 0)
            {
                std::string file = path + "/" + mp->specular_texname;
                specularTexture = ITexture2D::Create(mp->specular_texname, file, TextureType::eSPECULAR);
            }

            std::shared_ptr<ITexture2D> normalTexture;
            if (mp->normal_texname.length() > 0)
            {
                std::string file = path + "/" + mp->normal_texname;
                normalTexture = ITexture2D::Create(mp->normal_texname, file, TextureType::eNORMAL);
            }

            std::shared_ptr<ITexture2D> alphaTexture;
            if (mp->alpha_texname.length() > 0)
            {
                std::string file = path + "/" + mp->alpha_texname;
                alphaTexture = ITexture2D::Create(mp->alpha_texname, file, TextureType::eALPHAMASK);
            }

            std::shared_ptr<IMaterial> mat = IMaterial::Create(mp->name,
                diffuseTexture.get(),
                specularTexture.get(),
                normalTexture.get(),
                alphaTexture.get());

            mat->Properties().ambient = glm::vec4{ mp->ambient[0],mp->ambient[1],mp->ambient[2],1.0};
			mat->Properties().diffuse = glm::vec4{ mp->diffuse[0],mp->diffuse[1],mp->diffuse[2],1.0 };
			mat->Properties().specular = glm::vec4{ mp->specular[0],mp->specular[1],mp->specular[2],1.0 };
			mat->Properties().shininess = mp->shininess;
			mat->Properties().opacity = 0.0f;

            return mat;
        }
	}

    void Model::CalculateTangents(std::vector<Vertex>& verts, const std::vector<uint32_t>& indices)
	{
		
	}

    //void Model::CalculateTangents(std::vector<Vertex>& verts, const std::vector<uint32_t>& indices)
    //{
    //    int vertexCount = verts.size();
    //    int triangleCount = indices.size() / 3;

    //    std::vector<glm::vec4> tangents(vertexCount);
    //    std::vector<glm::vec3> tan1(vertexCount);
    //    std::vector<glm::vec3> tan2(vertexCount);

    //    int tri = 0;
    //    for (int i = 0; i < (triangleCount); i++)
    //    {
    //        int i1 = indices[tri];
    //        int i2 = indices[tri + 1];
    //        int i3 = indices[tri + 2];

    //        glm::vec3 v1 = verts[i1].Position;
    //        glm::vec3 v2 = verts[i2].Position;
    //        glm::vec3 v3 = verts[i3].Position;

    //        glm::vec2 w1 = verts[i1].TexCoord;
    //        glm::vec2 w2 = verts[i2].TexCoord;
    //        glm::vec2 w3 = verts[i3].TexCoord;

    //        float x1 = v2.x - v1.x;
    //        float x2 = v3.x - v1.x;
    //        float y1 = v2.y - v1.y;
    //        float y2 = v3.y - v1.y;
    //        float z1 = v2.z - v1.z;
    //        float z2 = v3.z - v1.z;

    //        float s1 = w2.x - w1.x;
    //        float s2 = w3.x - w1.x;
    //        float t1 = w2.y - w1.y;
    //        float t2 = w3.y - w1.y;

    //        float r = 1.0f / (s1 * t2 - s2 * t1);
    //        glm::vec3 sdir = glm::vec3((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
    //        glm::vec3 tdir = glm::vec3((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);

    //        tan1[i1] += sdir;
    //        tan1[i2] += sdir;
    //        tan1[i3] += sdir;

    //        tan2[i1] += tdir;
    //        tan2[i2] += tdir;
    //        tan2[i3] += tdir;

    //        tri += 3;
    //    }

    //    for (int i = 0; i < (vertexCount); i++)
    //    {
    //        glm::vec3 n = verts[i].Normal;
    //        glm::vec3 t = tan1[i];

    //        // Gram-Schmidt orthogonalize
    //        //t = glm::orthonormalize(n, t);
    //        // Gram-Schmidt orthogonalize
    //        t = glm::normalize(t - n * glm::dot(n, t));
    //        //t = glm::orthonormalize(t, n);


    //        tangents[i].x = t.x;
    //        tangents[i].y = t.y;
    //        tangents[i].z = t.z;

    //        // Calculate handedness
    //        tangents[i].w = (glm::dot(glm::cross(t, n), tan2[i]) < 0.0f) ? 1.0f : -1.0f;

    //        glm::vec3 b = glm::cross(n, t) * tangents[i].w;

    //        verts[i].Tangent = t;
    //        verts[i].Bitangent = b;
    //    }

    //    //// Calculate tangent and bitangent for each triangle and add to all three vertices.
    //    //for (int k = 0; k < indices.size(); k+=3)
    //    //{
    //    //    int i0 = indices[k];
    //    //    int i1 = indices[k + 1];
    //    //    int i2 = indices[k + 2];

    //    //    const glm::vec3& p0 = verts[i0].Position;
    //    //    const glm::vec3& p1 = verts[i1].Position;
    //    //    const glm::vec3& p2 = verts[i2].Position;
    //    //    const glm::vec2& w0 = verts[i0].TexCoord;
    //    //    const glm::vec2& w1 = verts[i1].TexCoord;
    //    //    const glm::vec2& w2 = verts[i2].TexCoord;

    //    //    glm::vec3 e1 = p1 - p0, e2 = p2 - p0;
    //    //    float x1 = w1.x - w0.x, x2 = w2.x - w0.x;
    //    //    float y1 = w1.y - w0.y, y2 = w2.y - w0.y;

    //    //    float r = 1.0F / (x1 * y2 - x2 * y1);
    //    //    glm::vec3 t = (e1 * y2 - e2 * y1) * r;
    //    //    glm::vec3 b = (e2 * x1 - e1 * x2) * r;

    //    //    verts[i0].Tangent += t;
    //    //    verts[i1].Tangent += t;
    //    //    verts[i2].Tangent += t;
    //    //    verts[i0].Bitangent += b;
    //    //    verts[i1].Bitangent += b;
    //    //    verts[i2].Bitangent += b;
    //    //}

    //    //// Orthonormalize each tangent and calculate the handedness.
    //    //for (int i = 0; i < verts.size(); i++)
    //    //{
    //    //    const glm::vec3& t = verts[i].Tangent;
    //    //    const glm::vec3& b = verts[i].Bitangent;
    //    //    const glm::vec3& n = verts[i].Normal;

    //    //    ////verts[i].Tangent = glm::normalize(glm::reflect(t, n));
    //    //    //verts[i].Tangent = glm::normalize(verts[i].Tangent);
    //    //    //verts[i].Bitangent = glm::normalize(verts[i].Bitangent);

    //    //    //if (glm::dot(glm::cross(verts[i].Normal, verts[i].Tangent), verts[i].Bitangent) < 0.0f) 
    //    //    //{
    //    //    //    verts[i].Tangent = verts[i].Tangent * -1.0f;
    //    //    //}

    //    //    //verts[i].Tangent = glm::normalize(t - n * glm::dot(n, t));
    //    //    verts[i].Tangent = glm::orthonormalize(n, t);

    //    //    if (glm::dot(glm::cross(n, t), b) < 0.0f) {
    //    //        verts[i].Tangent = t * -1.0f;
    //    //    }
    //}
}
