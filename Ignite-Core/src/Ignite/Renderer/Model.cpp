#include "igpch.h"
#include "Ignite/Renderer/Model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Ignite/Log.h"
#include "Ignite/Renderer/Renderer.h"

namespace Ignite
{
	std::unique_ptr<Model> Model::Load(const std::string& path)
	{
        std::unique_ptr<Model> model = std::unique_ptr<Model>(new Model);
        model->loadModel(path);
		if(!model->m_meshes.empty())
			return model;

        return std::unique_ptr<Model>();
	}
	
	const std::vector<std::shared_ptr<IMesh>>& Model::Meshes() const
	{
		return m_meshes;
	}

	void Model::loadModel(const std::string& path)
	{
		Assimp::Importer import;
		const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			LOG_CORE_FATAL("ERROR::ASSIMP::" + std::string(import.GetErrorString()));
            return;
		}

		processNode(scene->mRootNode, scene);
	}

	void Model::processNode(aiNode* node, const aiScene* scene)
	{
		// process all the node's meshes (if any)
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			m_meshes.push_back(processMesh(mesh, scene));
		}
		// then do the same for each of its children
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}

    std::shared_ptr<IMesh> Model::processMesh(aiMesh* mesh, const aiScene* scene)
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

        std::vector<std::shared_ptr<ITexture2D>> diffuseMaps;
        std::vector<std::shared_ptr<ITexture2D>> specularMaps;

        
        if (mesh->mMaterialIndex >= 0)
        {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

            //load diffuse textures
            std::vector<std::shared_ptr<ITexture2D>> diffuseMaps = loadMaterialTextures(material,
                aiTextureType_DIFFUSE, TextureType::eDIFFUSE);
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

            //load specular textures
            std::vector<std::shared_ptr<ITexture2D>> specularMaps = loadMaterialTextures(material,
                aiTextureType_SPECULAR, TextureType::eSPECULAR);
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end()); 
        }

		//finaly create mesh
        return IMesh::Create(vertices, indices, textures);
	}

	void Model::loadMaterials(const aiScene* aScene)
	{
        m_materials.resize(aScene->mNumMaterials);

        for (size_t i = 0; i < m_materials.size(); i++)
        {
            //m_materials[i] = IMaterial::Create(m)
        }
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
