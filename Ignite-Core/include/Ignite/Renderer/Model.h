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
        static std::unique_ptr<Model> Load(const std::string& path, const std::string& file);
    	
        const std::vector<std::shared_ptr<IMesh>>& Meshes() const;
        const std::vector<std::shared_ptr<IMaterial>>& Materials() const;
    private:
        std::vector<std::shared_ptr<IMesh>> m_meshes;
        std::vector<std::shared_ptr<IMaterial>> m_materials;

        void loadModel(const std::string& path, const std::string& file);
        void processNode(aiNode* node, const aiScene* scene, const std::string& path);
        std::shared_ptr<IMesh> processMesh(aiMesh* mesh, const aiScene* scene, const std::string& path);
        std::shared_ptr<IMaterial> getMaterial(const aiMaterial* material, const std::string& path);
        std::vector<std::shared_ptr<ITexture2D>> loadMaterialTextures(aiMaterial* mat, aiTextureType type, TextureType textureType);
    };
}
