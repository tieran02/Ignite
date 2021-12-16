#pragma once
#include "Mesh.h"
#include "Ignite/CreateProperties.h"

namespace tinyobj
{
    struct attrib_t;
    struct shape_t;
    struct material_t;
}

namespace Ignite
{
	struct ModelCreateInfo : SceneObjectCreateInfo, CreateProperties
	{
		ModelCreateInfo(const std::string& m_name, const std::string& m_path, const std::string& m_file)
			: SceneObjectCreateInfo(SceneObjectType::MODEL),
			  CreateProperties(CreatePropertyType::Model),
              m_name(m_name),
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
	
    class Model : public SceneObject
	{
    private:
        Model(const ModelCreateInfo& info);
    public:
        static Ref<Model> Create(const ModelCreateInfo& info);
    	
        const std::vector<Ref<Mesh>>& Meshes() const;
        const std::vector<Ref<Material>>& Materials() const;
    private:
        const ModelCreateInfo m_modelInfo;
        std::vector<Ref<Mesh>> m_meshes;
        std::vector<Ref<Material>> m_materials;

        void loadModel();
		Ref<Mesh> processMesh(const tinyobj::attrib_t& attrib, const tinyobj::shape_t& shape, const std::vector<tinyobj::material_t>& materials, const std::string& path);
        Ref<Material> getMaterial(const std::vector<tinyobj::material_t>& materials, int materialID, const std::string& path);
    };
}
