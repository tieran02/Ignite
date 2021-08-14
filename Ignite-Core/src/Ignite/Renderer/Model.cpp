#include "igpch.h"
#include "Ignite/Renderer/Model.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "Ignite/Log.h"
#include "Ignite/Renderer/Renderer.h"
#include "glm/gtc/type_ptr.inl"
#include "Ignite/Renderer/Material.h"
#include "Ignite/Renderer/MeshData.h"

namespace Ignite
{
    Model::Model(const ModelCreateInfo& info) :
		SceneObject(SceneObjectType::MODEL),
		m_modelInfo(info)
    {
    }
	
	std::unique_ptr<Model> Model::Create(const ModelCreateInfo& info)
	{
        std::unique_ptr<Model> model = std::unique_ptr<Model>(new Model(info));
        model->loadModel();
		if(!model->m_meshes.empty())
			return model;

        return std::unique_ptr<Model>();
	}


	const std::vector<const Mesh*>& Model::Meshes() const
	{
		return m_meshes;
	}

	const std::vector<const Material*>& Model::Materials() const
	{
        return m_materials;
	}

    void Model::loadModel()
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        std::string fullPath = m_modelInfo.GetPath() + "/" + m_modelInfo.GetFile();
        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, fullPath.c_str(), m_modelInfo.GetPath().c_str()))
        {
            CORE_ASSERT(false, warn + err);
        }

        for (const auto& shape : shapes)
        {
			m_meshes.push_back(processMesh(attrib, shape, materials, m_modelInfo.GetPath()));
        }
    }

    const Mesh* Model::processMesh(const tinyobj::attrib_t& attrib, const tinyobj::shape_t& shape, const std::vector<tinyobj::material_t>& materials, const std::string& path)
    {	
        std::vector<uint32_t> indices;
        std::vector<std::shared_ptr<Texture2D>> textures;

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


        const Material* material;

        // Vulkan viewer does not support texturing with per-face material.
        if (!shape.mesh.material_ids.empty() && shape.mesh.material_ids[0] >= 0)
        {
            material = getMaterial(materials, shape.mesh.material_ids[0], path);
        }
    	else
        {
            //make default mat
            material = Material::DefaultMaterial();
        }
        m_materials.push_back(material);
		//finaly create mesh

        meshData.CalculateTangents();

        return Renderer::GraphicsContext()->CreateMesh(MeshCreateInfo{ shape.name, std::move(meshData), material });
	}

	const Material* Model::getMaterial(const std::vector<tinyobj::material_t>& materials, int materialID,
		const std::string& path)
	{
        if(materialID < materials.size())
        {
            const tinyobj::material_t* mp = &materials[materialID];

            const Texture2D* diffuseTexture{ nullptr };
            if (mp->diffuse_texname.length() > 0)
			{
				std::string file = path + "/" + mp->diffuse_texname;
                Texture2DCreateInfo textureInfo{ mp->diffuse_texname, file, TextureType::eDIFFUSE };
                diffuseTexture = Renderer::GraphicsContext()->CreateTexture2D(textureInfo);
			}

            const Texture2D* specularTexture{ nullptr };
            if (mp->specular_texname.length() > 0)
            {
                std::string file = path + "/" + mp->specular_texname;
                Texture2DCreateInfo textureInfo{ mp->specular_texname, file, TextureType::eSPECULAR };
                specularTexture = Renderer::GraphicsContext()->CreateTexture2D(textureInfo);
            }

            const Texture2D* normalTexture{ nullptr };
            if (mp->normal_texname.length() > 0)
            {
                std::string file = path + "/" + mp->normal_texname;
                Texture2DCreateInfo textureInfo{ mp->normal_texname, file, TextureType::eNORMAL };
                normalTexture = Renderer::GraphicsContext()->CreateTexture2D(textureInfo);
            }

            const Texture2D* alphaTexture{ nullptr };
            if (mp->alpha_texname.length() > 0)
            {
                std::string file = path + "/" + mp->alpha_texname;
                Texture2DCreateInfo textureInfo{ mp->alpha_texname, file, TextureType::eALPHAMASK };
                alphaTexture = Renderer::GraphicsContext()->CreateTexture2D(textureInfo);
            }

            MaterialProperties properties
            {
                glm::vec4{ mp->ambient[0],mp->ambient[1],mp->ambient[2],1.0},
                glm::vec4{ mp->diffuse[0],mp->diffuse[1],mp->diffuse[2],1.0 },
                glm::vec4{ mp->specular[0],mp->specular[1],mp->specular[2],1.0 },
                 mp->shininess,
                0.0f
            };
        	
            MaterialCreateInfo createInfo
            {
                mp->name,
                std::move(properties),
                diffuseTexture,
                specularTexture,
                normalTexture,
                alphaTexture
            };

            return Renderer::GraphicsContext()->CreateMaterial(createInfo);;
        }
        return Material::DefaultMaterial();;
	}
}
