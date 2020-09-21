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
	struct ModelInfo
	{
		ModelInfo(const std::string& m_name, const std::string& m_path, const std::string& m_file)
			: m_name(m_name),
			  m_path(m_path),
			  m_file(m_file)
		{
		}

		const std::string& GetName() const { return m_name; }
		const std::string& GetPath() const { return m_path; }
		const std::string& GetFile() const { return m_file; }

	private:
        const std::string m_name;
        const std::string m_path;
        const std::string m_file;
	};
	
    class Model
	{
    private:
        Model(const ModelInfo& info);
    public:
        static std::unique_ptr<Model> Create(const ModelInfo& info);
    	
        const std::vector<std::shared_ptr<IMesh>>& Meshes() const;
        const std::vector<std::shared_ptr<IMaterial>>& Materials() const;
    private:
        const ModelInfo m_modelInfo;
        std::vector<std::shared_ptr<IMesh>> m_meshes;
        std::vector<std::shared_ptr<IMaterial>> m_materials;

        void loadModel();
        std::shared_ptr<IMesh> processMesh(const tinyobj::attrib_t& attrib, const tinyobj::shape_t& shape, const std::vector<tinyobj::material_t>& materials, const std::string& path);
        std::shared_ptr<IMaterial> getMaterial(const std::vector<tinyobj::material_t>& materials, int materialID, const std::string& path);
    };
}
