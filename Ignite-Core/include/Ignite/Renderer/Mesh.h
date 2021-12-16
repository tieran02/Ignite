#pragma once
#include "Ignite/CreateProperties.h"
#include "Buffer.h"
#include "Texture2D.h"
#include "MeshData.h"
#include "Vertex.h"
#include "Ignite/SceneObject.h"

namespace Ignite
{
	struct MeshData;
	class PipelineInputLayout;

	struct MeshCreateInfo : SceneObjectCreateInfo, CreateProperties
	{
	public:
		MeshCreateInfo(const std::string& name,MeshData&& mesh_data, const Ref<Material>& material)
			: SceneObjectCreateInfo(SceneObjectType::MESH),
			CreateProperties(CreatePropertyType::Mesh),
			m_name(name),
			m_meshData(std::move(mesh_data)),
			m_material(material)
		{
		}

		const std::string& GetName() const { return m_name; }
		const MeshData& GetMeshData() const { return m_meshData; }
		const Ref<Material>& GetMaterial() const { return m_material; }
		
	private:
		const std::string m_name;
		const MeshData m_meshData;
		Ref<Material> m_material;
	};
	
	class Mesh : public SceneObject
	{
	protected:
		Mesh(const MeshCreateInfo& meshInfo);
		virtual void Init(const MeshData& meshData) = 0;
		virtual void Cleanup() = 0;
	public:
		virtual ~Mesh() = default;
		virtual void BindDescriptors() const = 0;
		const GraphicsContext* m_context;

		static std::unique_ptr<Mesh> Create(const MeshCreateInfo& meshInfo);

		const Ref<Buffer>& VertexBuffer() const { return m_vertexBuffer; }
		const Ref<Buffer>& IndexBuffer() const { return m_IndexBuffer; }
		const MeshCreateInfo& MeshInfo() const { return m_meshInfo; }
		const Ref<Material>& Material() const { return m_meshInfo.GetMaterial(); }
		uint32_t IndexCount() const { return m_indexCount; }
	protected:
		Ref<Buffer> m_vertexBuffer;
		Ref<Buffer> m_IndexBuffer;
		uint32_t m_indexCount;

		MeshCreateInfo m_meshInfo;
	};
}
