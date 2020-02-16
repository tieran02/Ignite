#include "igpch.h"
#include "Ignite/Renderer/ModelLoader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Ignite/Log.h"
#include "Ignite/Renderer/Vertex.h"

Ignite::ModelLoader::ModelLoader(const std::string& path)
{
    LoadModel(path);
}

Ignite::ModelLoader::~ModelLoader()
{
}

std::unique_ptr<Ignite::ModelLoader> Ignite::ModelLoader::Load(const std::string& path)
{
    std::unique_ptr<ModelLoader> model = std::unique_ptr<ModelLoader>(new ModelLoader(path));
    return model;
}

void Ignite::ModelLoader::LoadModel(const std::string& path)
{
    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        LOG_CORE_FATAL("ERROR::ASSIMP::" + std::string(import.GetErrorString()));
    }

	//get first mesh
     // process all the node's meshes (if any)
    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[i];
    	
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
        	
            vertex.Position = {
                    mesh->mVertices[i].x,
                    mesh->mVertices[i].y,
                    mesh->mVertices[i].z
            };

            vertex.Normal = {
                    mesh->mNormals[i].x,
                    mesh->mNormals[i].y,
                    mesh->mNormals[i].z
            };

            if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
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
    }
}