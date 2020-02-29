#include "igpch.h"
#include "Ignite/Renderer/Model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Ignite/Log.h"
#include "Ignite/Renderer/Renderer.h"
#include "glm/gtc/type_ptr.inl"

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
		Assimp::Importer import;
		const aiScene* scene = import.ReadFile(path + "/" + file, aiProcess_Triangulate | aiProcess_FlipUVs);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			LOG_CORE_FATAL("ERROR::ASSIMP::" + std::string(import.GetErrorString()));
            return;
		}

		processNode(scene->mRootNode, scene, path);
	}

	void Model::processNode(aiNode* node, const aiScene* scene, const std::string& path)
	{
		// process all the node's meshes (if any)
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			m_meshes.push_back(processMesh(mesh, scene,path));
		}
		// then do the same for each of its children
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene,path);
		}
	}

    std::shared_ptr<IMesh> Model::processMesh(aiMesh* mesh, const aiScene* scene, const std::string& path)
    {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        std::vector<std::shared_ptr<ITexture2D>> textures;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;

            vertex.Position = {
                    mesh->mVertices[i].x,
                    mesh->mVertices[i].y,
                    mesh->mVertices[i].z
            };

            if (mesh->HasNormals()) // does the mesh contain texture coordinates?
            {
                vertex.Normal = {
                        mesh->mNormals[i].x,
                        mesh->mNormals[i].y,
                        mesh->mNormals[i].z
                };
            }
            else
            {
                vertex.Normal = { 0.0f,0.0f,0.0f };
            }

            if (mesh->HasTextureCoords(0)) // does the mesh contain texture coordinates?
            {
                vertex.TexCoord =
                {
                       mesh->mTextureCoords[0][i].x,
                       mesh->mTextureCoords[0][i].y
                };
            }
            else
            {
                vertex.TexCoord = { 0.0f,0.0f };
            }
            vertices.push_back(vertex);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        std::shared_ptr<IMaterial> material;
		if(mesh->mMaterialIndex > 0)
		{
            const aiMaterial* aiMaterial = scene->mMaterials[mesh->mMaterialIndex];
            material = getMaterial(aiMaterial, path);
		}
        else
        {
	        //make default mat
            material = IMaterial::Create("default", nullptr);
        }
		
		//finaly create mesh
        return IMesh::Create(vertices, indices, material);
	}

	std::shared_ptr<IMaterial> Model::getMaterial(const aiMaterial* material, const std::string& path)
	{
        aiString name;
        material->Get(AI_MATKEY_NAME, name);

        // Properties
        aiColor4D color;
        material->Get(AI_MATKEY_COLOR_AMBIENT, color);
        glm::vec4 ambient = glm::make_vec4(&color.r) + glm::vec4(0.1f);
        material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
        glm::vec4 diffuse = glm::make_vec4(&color.r);
        material->Get(AI_MATKEY_COLOR_SPECULAR, color);
        glm::vec4 specular = glm::make_vec4(&color.r);
        float opacity;
        material->Get(AI_MATKEY_OPACITY, opacity);
        float shininess;
        material->Get(AI_MATKEY_SHININESS, shininess);

        aiString str;
        std::shared_ptr<ITexture2D> diffuseTexture;
        if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
        {
            material->GetTexture(aiTextureType_DIFFUSE, 0, &str);
            std::string file = path + "/" + str.C_Str();
            diffuseTexture = ITexture2D::Create(str.C_Str(), file, TextureType::eDIFFUSE);
        }
        std::shared_ptr<ITexture2D> specularTexture;
		if (material->GetTextureCount(aiTextureType_SPECULAR) > 0)
        {
            material->GetTexture(aiTextureType_SPECULAR, 0, &str);
            std::string file = path + "/" + str.C_Str();
            specularTexture = ITexture2D::Create(str.C_Str(), file, TextureType::eSPECULAR);
        }

        std::shared_ptr<IMaterial> mat = IMaterial::Create(name.C_Str(), diffuseTexture.get(), specularTexture.get());

        mat->Properties().ambient = ambient;
        mat->Properties().diffuse = diffuse;
        mat->Properties().specular = specular;
        mat->Properties().shininess = shininess;
        mat->Properties().opacity = opacity;
		
        return mat;
	}

	std::vector<std::shared_ptr<ITexture2D>> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type,
        TextureType textureType)
	{
        std::vector<std::shared_ptr<ITexture2D>> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            std::shared_ptr<ITexture2D> texture = ITexture2D::Create(str.C_Str(), str.C_Str(), textureType);
            textures.push_back(texture);
        }
        return textures;
	}
}
