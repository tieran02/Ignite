#pragma once
#include "IMesh.h"

namespace tinyobj
{
    struct attrib_t;
    struct shape_t;
    struct material_t;
}

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
        std::shared_ptr<IMesh> processMesh(const tinyobj::attrib_t& attrib, const tinyobj::shape_t& shape, const std::vector<tinyobj::material_t>& materials, const std::string& path);
        std::shared_ptr<IMaterial> getMaterial(const std::vector<tinyobj::material_t>& materials, int materialID, const std::string& path);
    };
}
