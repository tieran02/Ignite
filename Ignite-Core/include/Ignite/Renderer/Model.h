#pragma once
#include "IMesh.h"

struct aiMaterial;
struct aiNode;
struct aiMesh;
struct aiScene;
enum aiTextureType;

namespace Ignite
{
    class Model
	{
    private:
        Model() = default;
    public:
        static std::unique_ptr<Model> Load(const std::string& path);
    	
        const std::vector<std::shared_ptr<IMesh>>& Meshes() const;
    private:
        std::vector<std::shared_ptr<IMesh>> m_meshes;
        std::vector<std::shared_ptr<IMaterial>> m_materials;

        void loadModel(const std::string& path);
        void processNode(aiNode* node, const aiScene* scene);
        std::shared_ptr<IMesh> processMesh(aiMesh* mesh, const aiScene* scene);
        void loadMaterials(const aiScene* aScene);
        std::vector<std::shared_ptr<ITexture2D>> loadMaterialTextures(aiMaterial* mat, aiTextureType type, TextureType textureType);
    };
}
